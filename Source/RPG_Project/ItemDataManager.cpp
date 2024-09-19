// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDataManager.h"

void UItemDataManager::InitializeData(UDataTable* DataTable)
{
    FString ContextString;

    for (const auto& RowName : DataTable->GetRowNames())
    {
        FItemData* ItemRow = DataTable->FindRow<FItemData>(RowName, ContextString);
        if (ItemRow)
        {
            ItemDataMap.Add(RowName, *ItemRow);
        }
    }
}

FItemData* UItemDataManager::GetItemData(FName ItemName)
{
    return ItemDataMap.Find(ItemName);
}

void UItemDataManager::UpdateItemMaxStack(FName ItemName, int32 NewMaxStack)
{
    FItemData* ItemData = GetItemData(ItemName);
    if (ItemData)
    {
        ItemData->MaxQuantity = NewMaxStack;
        UE_LOG(LogTemp, Warning, TEXT("Updated MaxStackSize for %s to %d"), *ItemName.ToString(), ItemData->MaxQuantity);
    }
}