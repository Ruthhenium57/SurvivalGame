// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"

#include "InventoryDataSubsystem.h"
#include "Components/ScrollBox.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "Inventory/MainItemActor.h"
#include "InventoryLogMacros.h"
#include "Inventory/InventoryComponent.h"
#include "InventorySystemPlugin.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeAll();
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
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UInventoryWidget::InitializeAll, 0.1f, false);
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

	TArray<FName> ServerSlotNames;
	for (const FItemInventorySlot& ItemSlot : InventoryComponent->GetInventorySlots())
	{
		ServerSlotNames.Add(ItemSlot.ItemClass->GetFName());
		if (UItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemClass->GetFName()))
		{
			UItemSlotWidget* Widget = *WidgetPtr;
			if (Widget->IsPredicted() || Widget->GetQuantity() != ItemSlot.Quantity)
			{
				UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
				Widget->UpdateItemInfo(ItemSlot.ItemClass, ItemSlot.Quantity, InventorySubsystem->GetItemDataByClass(ItemSlot.ItemClass));
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

void UInventoryWidget::OnItemActionRejected(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		FItemInventorySlot* InventorySlot = InventoryComponent->FindSlotByClass(ItemClass);
		UItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemClass->GetFName()); // find widget slot in list
		if (WidgetPtr)
		{
			UItemSlotWidget* Widget = *WidgetPtr;
			if (Widget->IsPredicted())
			{
				if (InventorySlot)
				{
					RefreshSlotInfo(*InventorySlot, false);
				}
				else
				{
					FItemInventorySlot WidgetSlot;
					WidgetSlot.ItemClass = ItemClass;
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
	if (ItemSlot.ItemClass && InventoryComponent)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemClass->GetFName(), Delegate);
		}
		UItemSlotWidget** ItemWidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemClass->GetFName()); // find slot widget in map
		if (ItemWidgetPtr)
		{
			UItemSlotWidget* ItemWidget = *ItemWidgetPtr;
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
			FItemData ItemData =InventorySubsystem->GetItemDataByClass(ItemSlot.ItemClass);
			ItemWidget->UpdateItemInfo(ItemSlot.ItemClass, ItemSlot.Quantity, ItemData); // update slot info
			ItemWidget->SetIsPredicted(IsPredicted);
			LOGF_INV(Display, "Widget has been updated");
			OnRefreshInfo(ItemSlot);
		}
		LOGF_INV(Error, "Cant find the slot when synced it with server");
	}
}

void UInventoryWidget::AddNewSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted)
{
	if (ItemSlot.ItemClass)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemClass->GetFName(), Delegate);
		}
		if (UItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemClass->GetFName()))
		{
			UItemSlotWidget* Widget = *WidgetPtr;
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
			FItemData ItemData = InventorySubsystem->GetItemDataByClass(ItemSlot.ItemClass);
			Widget->UpdateItemInfo(ItemSlot.ItemClass, ItemSlot.Quantity, ItemData);
			Widget->SetIsPredicted(false);
			LOGF_INV(Display, "Slot was synced with server");
			return;
		}
		
		if (UItemSlotWidget* Widget = CreateWidget<UItemSlotWidget>(this, ItemWidgetClass,
																	   FName(ItemSlot.ItemClass->GetName())))
		{	// create widget slot
			ItemSlotWidgets.Add(ItemSlot.ItemClass->GetFName(), Widget); // add to map
			InventoryList->AddChild(Widget); // add to widget slot list
			Widget->SetIsPredicted(IsPredicted);
			UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
			FItemData ItemData = InventorySubsystem->GetItemDataByClass(ItemSlot.ItemClass);
			Widget->UpdateItemInfo(ItemSlot.ItemClass, ItemSlot.Quantity, ItemData);
			// update slot info
			LOGF_INV(Display, "New slot has been added to widget");
			OnAddNewSlot(ItemSlot);
		}
	}
}

void UInventoryWidget::RemoveSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted)
{
	if (ItemSlot.ItemClass)
	{
		if (IsPredicted)
		{
			FDelegateHandle Delegate = InventoryComponent->OnItemActionRejected.AddUObject(this, &UInventoryWidget::OnItemActionRejected);
			PredictedActionRejectHandles.Add(ItemSlot.ItemClass->GetFName(), Delegate);
		}
		UItemSlotWidget** WidgetPtr = ItemSlotWidgets.Find(ItemSlot.ItemClass->GetFName()); // find widget slot in list
		if (WidgetPtr)
		{
			UItemSlotWidget* Widget = *WidgetPtr;
			InventoryList->RemoveChild(Widget); // remove widget
			ItemSlotWidgets.Remove(ItemSlot.ItemClass->GetFName()); // remove from map
			LOGF_INV(Display, "Slot has been removed from widget");
			OnRemoveSlot(ItemSlot);
			return;
		}
		LOGF_INV(Display, "Slot is really was removed from widget");
	}
}
