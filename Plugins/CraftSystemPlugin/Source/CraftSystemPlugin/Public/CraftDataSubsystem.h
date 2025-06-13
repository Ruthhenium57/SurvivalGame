// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Craft/CraftComponent.h"
#include "Craft/CraftData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CraftDataSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class CRAFTSYSTEMPLUGIN_API UCraftDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FCraftData* GetCraftItemData(int32 ItemID, ECraftType CraftType);

	TMap<int32, FCraftData>* GetCachedDataTable(ECraftType CraftType);
	
protected:
	TMap<ECraftType, TMap<int32, FCraftData>> CraftDataCachesMap;
	
	UFUNCTION()
	void CacheCraftDT();
	
	UPROPERTY()
	TMap<ECraftType, UDataTable*> CraftDataTables;
};
