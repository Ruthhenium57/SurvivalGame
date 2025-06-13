// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryItemSlotWidget.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryComponent.h"


void UInventoryItemSlotWidget::UpdateItemInfo(int32 ItemID, int32 Count, FItemData ItemData)
{
	if (ItemData.ItemClass && ItemID)
	{
		ItemName->SetText(FText::FromName(ItemData.ItemName));
		ItemQuantity->SetText(FText::AsNumber(Count));
		OnUpdateItemInfo(ItemID, Count, ItemData);
	}
}
