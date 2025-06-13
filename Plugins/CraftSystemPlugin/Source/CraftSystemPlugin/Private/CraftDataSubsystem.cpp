// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftDataSubsystem.h"
#include "CraftLogMacros.h"
#include "CraftSystemPlugin.h"
#include "InventoryDataSubsystem.h"
#include "Craft/CraftComponent.h"

void UCraftDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UInventoryDataSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
	if (Subsystem)
	{
		Subsystem->InventoryDataIsReadyDelegate.AddUObject(this, &UCraftDataSubsystem::CacheCraftDT);
	}
}

FCraftData* UCraftDataSubsystem::GetCraftItemData(int32 ItemID, ECraftType CraftType)
{
	if (!ItemID) return nullptr;

	TMap<int32, FCraftData>* CraftDataCache = CraftDataCachesMap.Find(CraftType);
	if (CraftDataCache)
	{
		TMap<int32, FCraftData> CraftDataCachePtr = *CraftDataCache;
		
		if (FCraftData* CraftData = CraftDataCachePtr.Find(ItemID))
			return CraftData;
		
		LOGF_CRAFT(Error, "No craft item data found for ID %d", ItemID);
		return nullptr;
	}
	LOGF_CRAFT(Error, "No craft item data found for craft type %s", *UEnum::GetValueAsString(CraftType));
	return nullptr;
}

TMap<int32, FCraftData>* UCraftDataSubsystem::GetCachedDataTable(ECraftType CraftType)
{
	return CraftDataCachesMap.Find(CraftType);
}

void UCraftDataSubsystem::CacheCraftDT()
{
	CraftDataTables.FindOrAdd(ECraftType::Hand,
	                          LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft")));
	//CraftDataTables.FindOrAdd(ECraftType::Workbench,
	//						  LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft")));
	
	// create cache
	for (ECraftType CraftType : TEnumRange<ECraftType>())
	{
		if (UDataTable** FoundTablePtr = CraftDataTables.Find(CraftType))
		{
			UDataTable* FoundTable = *FoundTablePtr;
			TMap<int32, FCraftData> CraftDataCache;
			for (const TPair<FName, uint8*>& Row : FoundTable->GetRowMap())
			{
				FCraftData* CraftData = reinterpret_cast<FCraftData*>(Row.Value);
				if (CraftData && CraftData->ItemToCraft) CraftDataCache.Add(CraftData->ItemToCraft, *CraftData);
			}
			CraftDataCachesMap.Add(CraftType, CraftDataCache);
		}
	}
	LOGF_CRAFT(Warning, "Craft items cache was created")
}
