// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InventoryDataSubsystem.generated.h"

class AMainItemActor;
/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API UInventoryDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemData GetItemDataByID(int32 ItemID);
	
	DECLARE_MULTICAST_DELEGATE(FInventoryDataIsReadyDelegate)
	FInventoryDataIsReadyDelegate InventoryDataIsReadyDelegate;
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CacheItemDataTable();

	UPROPERTY()
	TObjectPtr<UDataTable> ItemDataTable;

	UPROPERTY()
	TMap<int32, FItemData> ItemDataCache;
};