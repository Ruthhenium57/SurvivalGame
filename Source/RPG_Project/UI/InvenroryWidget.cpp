// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenroryWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UInvenroryWidget::UpdateSlotInfo(FItemInventorySlot ItemSlot)
{
	if (!ItemSlot.Items.IsEmpty())
	{
		for (UWidget* Widget : InventoryList->GetAllChildren())
		{
			if (Widget->GetName() == ItemSlot.ItemClass->GetName())
			{
				UItemSlotWidget* ItemWidget = Cast<UItemSlotWidget>(Widget);
				if (ItemWidget)
				{
					ItemWidget->UpdateItemInfo(ItemSlot);
					UE_LOG(LogTemp, Display, TEXT("Widget slot info updated"));
					return;
				}
			}
		}
		AddNewSlot(ItemSlot);
	}
	else
	{
		RemoveSlot(ItemSlot.ItemClass);
	}
}

void UInvenroryWidget::UpdateInventory(TArray<FItemInventorySlot> ItemSlots)
{
	
}

void UInvenroryWidget::AddNewSlot(FItemInventorySlot ItemSlot)
{
	if (!ItemSlot.Items.IsEmpty())
	{
		if (UItemSlotWidget* Widget = CreateWidget<UItemSlotWidget>(this, ItemWidgetClass, FName(ItemSlot.ItemClass->GetName())))
		{
			InventoryList->AddChild(Widget);
			Widget->UpdateItemInfo(ItemSlot);
			UE_LOG(LogTemp, Display, TEXT("NewSlotAddedToWidget"));
		}
	}
}

void UInvenroryWidget::RemoveSlot(TSubclassOf<AMainItemActor> ItemClass)
{
	for (UWidget* Widget : InventoryList->GetAllChildren())
	{
		if (Widget->GetName() == ItemClass->GetName())
		{
			InventoryList->RemoveChild(Widget);
			UE_LOG(LogTemp, Display, TEXT("SlotRemovedFromWidget"));
		}
	}
}

