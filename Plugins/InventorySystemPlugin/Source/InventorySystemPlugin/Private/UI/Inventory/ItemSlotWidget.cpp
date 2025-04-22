// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryComponent.h"


void UItemSlotWidget::UpdateItemInfo(TSubclassOf<AMainItemActor> ItemClass, int32 Count, FItemData ItemData)
{
	if (ItemData.ItemClass && ItemClass)
	{
		ItemName->SetText(FText::FromName(ItemData.ItemName));
		ItemQuantity->SetText(FText::AsNumber(Count));
		OnUpdateItemInfo(ItemClass, Count, ItemData);
	}
}
