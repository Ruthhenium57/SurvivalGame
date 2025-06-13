// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ControllableInterface.generated.h"

struct FIAConfig;
struct FInputActionValue;
class UInputAction;
// This class does not need to be modified.
UINTERFACE()
class UControllableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALPROJECT_API IControllableInterface
{
	GENERATED_BODY()

public:
	virtual void ExecuteInput(const UInputAction* IA, FIAConfig Cfg, const FInputActionValue& Value) = 0;
};
