// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryOwnerInterface.generated.h"

class UInventoryComponent;
// This class does not need to be modified.
UINTERFACE()
class UInventoryOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORYSYSTEMPLUGIN_API IInventoryOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UInventoryComponent* GetInventoryComponent() const = 0;
};
