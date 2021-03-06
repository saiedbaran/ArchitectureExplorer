#pragma once

#include "Misc/OutputDevice.h"
#include "Delegates/Delegate.h"

DECLARE_DELEGATE_FourParams(FOnSerializeMessage, const TCHAR*, ELogVerbosity::Type, const class FName&, TOptional<double>);

class FRiderOutputDevice : public FOutputDevice {
public:

	FRiderOutputDevice();

	~FRiderOutputDevice();

	FOnSerializeMessage onSerializeMessage;

protected:

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category) override;

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category, double Time) override;

private:
};
