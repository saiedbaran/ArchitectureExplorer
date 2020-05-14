// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "Components/PostProcessComponent.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
    VRRoot->SetupAttachment(GetRootComponent());

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    Camera->SetupAttachment(VRRoot);

    TeleportTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportTarget"));
    TeleportTarget->SetupAttachment(GetRootComponent());

    PostProcessingComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessingComponent"));
    PostProcessingComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
    Super::BeginPlay();

    Camera = Cast<UCameraComponent>(GetDefaultSubobjectByName(TEXT("CameraComponent")));
    PC = Cast<APlayerController>(GetController());

    BlinkerSetup();
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CameraCalibration();
    RayCasting();
}

void AVRCharacter::InputBinding(UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVRCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVRCharacter::MoveRight);

    PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::BeginTeleport);
    PlayerInputComponent->BindAction(TEXT("MoveTest"), IE_Pressed, this, &AVRCharacter::TestMovement);
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    InputBinding(PlayerInputComponent);
}

void AVRCharacter::TestMovement()
{
    if(!Camera) {return;}
    UE_LOG(LogTemp,Warning,TEXT("Test Movement is working"));
    auto Throttle = 1.0f;
    AddMovementInput(Throttle * Camera->GetForwardVector());
}

void AVRCharacter::MoveForward(float Throttle)
{
    if (!Camera) { return; }
    AddMovementInput(Throttle * Camera->GetForwardVector());
}

void AVRCharacter::MoveRight(float Throttle)
{
    if (!Camera) { return; }
    AddMovementInput(Throttle * Camera->GetRightVector());
}

void AVRCharacter::BeginTeleport()
{
    StartFade(0,1);

    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, FadeDuration);
}

void AVRCharacter::FinishTeleport()
{
    const auto HeightOffset = FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
    SetActorLocation(TeleportTarget->GetComponentLocation() + HeightOffset);

    StartFade(1, 0);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
    if (PC != nullptr)
    {
        APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
        CameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeDuration, FLinearColor::Black);
    }
}

bool AVRCharacter::FindTeleportDestination(FVector& OutLocation)
{
    if (!Camera) { return false; }
    auto Start = Camera->GetComponentLocation();
    auto End = Start + Camera->GetForwardVector() * LineTraceMaxDistance;
    FHitResult OutHit;

    bool bIsLineTraced = GetWorld()->LineTraceSingleByChannel(
        OutHit,
        Start,
        End,
        ECC_Visibility
    );
    if (!bIsLineTraced) { return false; }
    DrawDebugLine(GetWorld(), Camera->GetComponentLocation(), OutHit.Location, FColor::Green);

    FNavLocation OutNavPoint;
    bool bOnNavMesh = UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(
        OutHit.Location,
        OutNavPoint,
        NavExtent
    );
    if (!bOnNavMesh) { return false; }

    OutLocation = OutNavPoint.Location;
    return true;
}

void AVRCharacter::BlinkerSetup()
{
    if (BlinkerMaterialBase != nullptr)
    {
        InstanceBlinkerMaterial = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
        PostProcessingComponent->AddOrUpdateBlendable(InstanceBlinkerMaterial);

        InstanceBlinkerMaterial->SetScalarParameterValue("Radius", 0.1);
    }
}

void AVRCharacter::CameraCalibration()
{
    if (!Camera) { return; }
    auto CameraLocation = Camera->GetComponentLocation();
    auto Offset = CameraLocation - GetActorLocation();
    Offset.Z = 0;

    AddActorWorldOffset(Offset);
    VRRoot->AddWorldOffset(-Offset);
}

void AVRCharacter::RayCasting()
{
    FVector Location;
    if (FindTeleportDestination(Location))
    {
        TeleportTarget->SetWorldLocation(Location);
        TeleportTarget->SetVisibility(true);
    }
    else
    {
        TeleportTarget->SetVisibility(false);
    }
}
