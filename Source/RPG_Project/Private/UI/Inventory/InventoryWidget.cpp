// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UInventoryWidget::Construct()
{
	Super::Construct();
}

void UInventoryWidget::UpdateSlotInfo(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass && OwningPlayer)
	{
		FItemInventorySlot ItemSlot = OwningPlayer->InventoryComponent->FindSlotByClass(ItemClass);  // find slot in player
		if (!ItemSlot.Items.IsEmpty())
		{
			UItemSlotWidget** ItemWidgetPtr = ItemSlotWidgets.Find(ItemClass->GetName());	// find slot widget in map
			if (ItemWidgetPtr)
			{
				UItemSlotWidget* ItemWidget = *ItemWidgetPtr;
				ItemWidget->UpdateItemInfo(ItemClass, ItemSlot.Items.Num());	// update slot info
				UE_LOG(LogTemp, Display, TEXT("UInventoryWidget: Widget slot info updated"));
				return;
			}
			else
			{
				AddNewSlot(ItemClass);	// add slot if is no
			}
		}
		else
		{
			RemoveSlot(ItemClass);  // remove slot if is empty
		}
	}
}

void UInventoryWidget::UpdateInventory(TArray<FItemInventorySlot> ItemSlots)
{
	
}

void UInventoryWidget::AddNewSlot(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		if (UItemSlotWidget* Widget = CreateWidget<UItemSlotWidget>(this, ItemWidgetClass, FName(ItemClass->GetName()))) // create widget slot
		{
			ItemSlotWidgets.Add(ItemClass->GetName(), Widget); // add to map
			InventoryList->AddChild(Widget);   // add to widget slot list
			Widget->UpdateItemInfo(ItemClass, OwningPlayer->InventoryComponent->FindSlotByClass(ItemClass).Items.Num());	// update slot info
			UE_LOG(LogTemp, Display, TEXT("UInventoryWidget: NewSlotAddedToWidget"));
		}
	}
}

void UInventoryWidget::RemoveSlot(TSubclassOf<AMainItemActor> ItemClass)
{
	UItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemClass->GetName());	// find widget slot in list
	if (WidgetPtr)
	{
		UItemSlotWidget* Widget = *WidgetPtr;
		InventoryList->RemoveChild(Widget);		// remove widget
		ItemSlotWidgets.Remove(ItemClass->GetName());	// remove from map
		UE_LOG(LogTemp, Display, TEXT("UInventoryWidget: SlotRemovedFromWidget"));
	}
}

