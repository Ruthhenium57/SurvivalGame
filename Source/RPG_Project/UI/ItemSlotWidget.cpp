// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "InventoryComponent.h"


void UItemSlotWidget::UpdateItemInfo(FItemInventorySlot ItemSlot)
{
	FItemData* ItemData = nullptr;
	UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
	if (ItemDataTable)
	{
		FName RowName = FName(ItemSlot.ItemClass->GetName().RightChop(7).LeftChop(2));
		ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
		if (ItemData)
		{
			ItemName->SetText(FText::FromString(ItemData->ItemName));
			ItemQuantity->SetText(FText::AsNumber(ItemSlot.Items.Num()));
		}
	}
}
