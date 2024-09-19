// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemDataManager.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UItemDataManager : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TMap<FName, FItemData> ItemDataMap;

    void InitializeData(UDataTable* DataTable);

    FItemData* GetItemData(FName ItemName);

    void UpdateItemMaxStack(FName ItemName, int32 NewMaxStack);
};
