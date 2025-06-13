// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"
#include "InventoryDataSubsystem.h"
#include "Components/ScrollBox.h"
#include "UI/Inventory/InventoryItemSlotWidget.h"
#include "InventoryLogMacros.h"
#include "Inventory/InventoryComponent.h"
#include "InventorySystemPlugin.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UInventoryWidget::InitializeAll, 0.1f, false);
}

void UInventoryWidget::InitializeAll()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventorySlotAdded.AddUObject(this, &UInventoryWidget::AddNewSlot);
		InventoryComponent->OnInventorySlotRemoved.AddUObject(this, &UInventoryWidget::RemoveSlot);
		InventoryComponent->OnInventorySlotChanged.AddUObject(this, &UInventoryWidget::RefreshSlotInfo);
		UpdateInventory();
	}
}

void UInventoryWidget::UpdateInventory()
{
	if (InventoryComponent)
	{
		InventoryList->ClearChildren();
		ItemSlotWidgets.Empty();
		for (const FItemInventorySlot& InventorySlot : InventoryComponent->GetInventorySlots())
		{
			AddNewSlot(InventorySlot, false);
		}
	}
}

void UInventoryWidget::SyncInventoryWithServer()
{
	if (!InventoryComponent) return;

	TArray<int32> ServerSlotNames;
	for (const FItemInventorySlot& ItemSlot : InventoryComponent->GetInventorySlots())
	{
		ServerSlotNames.Add(ItemSlot.ItemID);
		if (UInventoryItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemID))
		{
			UInventoryItemSlotWidget* Widget = *WidgetPtr;
			if (Widget->IsPredicted() || Widget->GetQuantity() != ItemSlot.Quantity)
			{
				UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<
					UInventoryDataSubsystem>();
				Widget->UpdateItemInfo(ItemSlot.ItemID, ItemSlot.Quantity,
				                       InventorySubsystem->GetItemDataByID(ItemSlot.ItemID));
				Widget->SetIsPredicted(false);
			}
		}
		else
		{
			AddNewSlot(ItemSlot, false);
		}
	}

	for (auto It = ItemSlotWidgets.CreateIterator(); It; ++It)
	{
		if (It.Value()->IsPredicted() && !ServerSlotNames.Contains(It.Key()))
		{
			InventoryList->RemoveChild(It.Value());
			It.RemoveCurrent();
		}
	}
}

void UInventoryWidget::OnItemActionRejected(int32 ItemID)
{
	if (ItemID > 0)
	{
		FItemInventorySlot* InventorySlot = InventoryComponent->FindSlotByID(ItemID);
		UInventoryItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemID); // find widget slot in list
		if (WidgetPtr)
		{
			UInventoryItemSlotWidget* Widget = *WidgetPtr;
			if (Widget->IsPredicted())
			{
				if (InventorySlot)
				{
					RefreshSlotInfo(*InventorySlot, false);
				}
				else
				{
					FItemInventorySlot WidgetSlot;
					WidgetSlot.ItemID = ItemID;
					RemoveSlot(WidgetSlot, false);
				}
			}
			LOGF_INV(Warning, "Widget has been found but it is not predicted");
		}
		else
		{
			AddNewSlot(*InventorySlot, false);
		}
	}
}

void UInventoryWidget::RefreshSlotInfo(const FItemInventorySlot& ItemSlot, bool IsPredicted)
{
	if (ItemSlot.ItemID > 0 && InventoryComponent)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(
				this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemID, Delegate);
		}
		UInventoryItemSlotWidget** ItemWidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemID); // find slot widget in map
		if (ItemWidgetPtr)
		{
			UInventoryItemSlotWidget* ItemWidget = *ItemWidgetPtr;
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<
				UInventoryDataSubsystem>();
			FItemData ItemData = InventorySubsystem->GetItemDataByID(ItemSlot.ItemID);
			ItemWidget->UpdateItemInfo(ItemSlot.ItemID, ItemSlot.Quantity, ItemData); // update slot info
			ItemWidget->SetIsPredicted(IsPredicted);
			LOGF_INV(Display, "Widget has been updated");
			OnRefreshInfo(ItemSlot);
		}
		LOGF_INV(Error, "Cant find the slot when synced it with server");
	}
}

void UInventoryWidget::AddNewSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted)
{
	if (ItemSlot.ItemID > 0)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(
				this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemID, Delegate);
		}
		if (UInventoryItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemID))
		{
			UInventoryItemSlotWidget* Widget = *WidgetPtr;
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<
				UInventoryDataSubsystem>();
			FItemData ItemData = InventorySubsystem->GetItemDataByID(ItemSlot.ItemID);
			Widget->UpdateItemInfo(ItemSlot.ItemID, ItemSlot.Quantity, ItemData);
			Widget->SetIsPredicted(false);
			LOGF_INV(Display, "Slot was synced with server");
			return;
		}

		if (UInventoryItemSlotWidget* Widget = CreateWidget<UInventoryItemSlotWidget>(this, ItemWidgetClass, FName("ItemSlotWidget")))
		{
			// create widget slot
			ItemSlotWidgets.Add(ItemSlot.ItemID, Widget); // add to map
			InventoryList->AddChild(Widget); // add to widget slot list
			Widget->SetIsPredicted(IsPredicted);
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<
				UInventoryDataSubsystem>();
			FItemData ItemData = InventorySubsystem->GetItemDataByID(ItemSlot.ItemID);
			Widget->UpdateItemInfo(ItemSlot.ItemID, ItemSlot.Quantity, ItemData);
			// update slot info
			LOGF_INV(Display, "New slot has been added to widget");
			OnAddNewSlot(ItemSlot);
		}
	}
}

void UInventoryWidget::RemoveSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted)
{
	if (ItemSlot.ItemID > 0)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(
				this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemID, Delegate);
		}
		UInventoryItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemID); // find widget slot in list
		if (WidgetPtr)
		{
			UInventoryItemSlotWidget* Widget = *WidgetPtr;
			InventoryList->RemoveChild(Widget); // remove widget
			ItemSlotWidgets.Remove(ItemSlot.ItemID); // remove from map
			LOGF_INV(Display, "Slot has been removed from widget");
			OnRemoveSlot(ItemSlot);
			return;
		}
		LOGF_INV(Display, "Slot is really was removed from widget");
	}
}
