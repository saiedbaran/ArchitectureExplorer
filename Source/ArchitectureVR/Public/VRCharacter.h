// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class UCameraComponent;
class UNavigationSystemBase;
class UMaterialInterface;

UCLASS()
class ARCHITECTUREVR_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InputBinding(UInputComponent* PlayerInputComponent);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void CameraCalibration();

	void RayCasting();
	
	void MoveForward(float Throttle);
	void MoveRight(float Throttle);
	void TestMovement();
	
	void BeginTeleport();
	void FinishTeleport();
	void StartFade(float FromAlpha, float ToAlpha);
	bool FindTeleportDestination(FVector &OutLocation);

	void BlinkerSetup();

private:
	UPROPERTY()
	UCameraComponent* Camera;
	
	UPROPERTY()
	USceneComponent* VRRoot;

	UPROPERTY()
	APlayerController* PC;

	// Teleportation Parameters
	UPROPERTY(VisibleAnywhere, Category = "Teleportation")
	UStaticMeshComponent* TeleportTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Teleportation")
	float LineTraceMaxDistance = 10000;

	UPROPERTY(EditAnywhere, Category = "Teleportation")
	float FadeDuration = 1;

	// Navigation System
	UPROPERTY(EditAnywhere, Category= "NavigationSystem")
	FVector NavExtent = FVector(100,100,100);
	
	// Post Processing
	class UPostProcessComponent* PostProcessingComponent;

	UPROPERTY(EditAnywhere, Category = "PostProcessing")
	UMaterialInterface* BlinkerMaterialBase;

	UPROPERTY()
	UMaterialInstanceDynamic* InstanceBlinkerMaterial;
};
