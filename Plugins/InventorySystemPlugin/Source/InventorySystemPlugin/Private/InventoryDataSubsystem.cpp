// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryDataSubsystem.h"
#include "InventoryLogMacros.h"
#include "InventorySystemPlugin.h"

void UInventoryDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CacheItemDataTable();
}

FItemData UInventoryDataSubsystem::GetItemDataByID(int32 ItemID)
{
	if (!ItemID) return FItemData();
	FItemData* ItemData = ItemDataCache.Find(ItemID);
	if (ItemData) return *ItemData;
	LOGF_INV(Error, "No item data found for ID %d", ItemID);
	return FItemData();
}

void UInventoryDataSubsystem::CacheItemDataTable()
{
	if (!ItemDataTable)
	{
		// load item  DT
		ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Items.DT_Items"));
		if (!ItemDataTable)
		{
			LOGF_INV(Error, "Failed to load DT");
			return;
		}
	}
	// create cache
	for (const TPair<FName, uint8*>& Row : ItemDataTable->GetRowMap())
	{
		const FItemData* ItemData = reinterpret_cast<const FItemData*>(Row.Value);
		if (ItemData && ItemData->ItemClass)
		{
			ItemDataCache.Add(ItemData->ItemID, *ItemData);
		}
	}
	InventoryDataIsReadyDelegate.Broadcast();
}
