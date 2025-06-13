// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CraftOwnerInterface.generated.h"

class UCraftComponent;
// This class does not need to be modified.
UINTERFACE()
class UCraftOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRAFTSYSTEMPLUGIN_API ICraftOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UCraftComponent* GetCraftComponent() const = 0;
};
