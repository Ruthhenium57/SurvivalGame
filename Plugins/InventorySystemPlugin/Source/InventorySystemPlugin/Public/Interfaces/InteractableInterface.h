// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"


class UInventoryComponent;

UENUM(BlueprintType)
enum class EInteractType : uint8
{
	Take			UMETA(DisplayName = "Take"),
	Put				UMETA(DisplayName = "Put"),
	Open			UMETA(DisplayName = "Open"),
	Close			UMETA(DisplayName = "Close"),
	Use				UMETA(DisplayName = "Use"),
	Read			UMETA(DisplayName = "Read"),
	Inspect			UMETA(DisplayName = "Inspect"),
	Switch			UMETA(DisplayName = "Switch")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORYSYSTEMPLUGIN_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact(UInventoryComponent* InventoryComponent) = 0;
	virtual void SecondInteract(UInventoryComponent* InventoryComponent) {};
	virtual TArray<EInteractType> GetInteractTypes() = 0;
	virtual FName GetObjectName() = 0;
};
