// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "InventoryDataSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "InventoryLogMacros.h"
#include "InventorySystemPlugin.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, InventorySlots, COND_OwnerOnly);
}

FItemInventorySlot* UInventoryComponent::FindSlotByID(int32 ItemID)
{
	if (!ItemID) return nullptr;

	for (FItemInventorySlot& Slot : InventorySlots.Items)
	{
		if (Slot.ItemID == ItemID) return &Slot;
	}

	return nullptr;
}

const TArray<FItemInventorySlot>& UInventoryComponent::GetInventorySlots()
{
	return InventorySlots.Items;
}

void UInventoryComponent::ClientPredictAddItem(int32 ItemID, int32 AddAmount)
{
	if (CanAddItems(ItemID, AddAmount))
	{
		if (!GetOwner()->HasAuthority())
		{
			FItemInventorySlot* Slot = FindSlotByID(ItemID);
			if (Slot)
			{
				OnInventorySlotChanged.Broadcast(*Slot, true);
				return;
			}
			FItemInventorySlot NewSlot;
			NewSlot.ItemID = ItemID;
			NewSlot.Quantity = AddAmount;
			NewSlot.MaxQuantity = InventoryDataSubsystem->GetItemDataByID(ItemID).MaxQuantity;
			OnInventorySlotAdded.Broadcast(NewSlot, true);
		}
	}
}

void UInventoryComponent::ClientPredictRemoveItem(int32 ItemID, int32 RemoveAmount)
{
	if (!GetOwner()->HasAuthority())
	{
		FItemInventorySlot* Slot = FindSlotByID(ItemID);
		if (Slot->Quantity == RemoveAmount)
		{
			OnInventorySlotRemoved.Broadcast(*Slot, true);
		}
		else
		{
			OnInventorySlotChanged.Broadcast(*Slot, true);
		}
	}
}

void UInventoryComponent::AddItemByInstances(const TArray<AMainItemActor*>& Items)
{
	TMap<int32, TArray<AMainItemActor*>> SortedItems;
	for (AMainItemActor* Item : Items)
	{
		if (!Item) continue;
		TArray<AMainItemActor*>* ExistingItems = SortedItems.Find(Item->ItemID);
		if (ExistingItems)
		{
			ExistingItems->Add(Item);
		}
		else
		{
			TArray<AMainItemActor*> NewArray;
			NewArray.Add(Item);
			SortedItems.Add(Item->ItemID, NewArray);
		}
	}
	for (auto Slot : SortedItems)
	{
		if (CanAddItems(Slot.Key, Slot.Value.Num()))
		{
			ServerAddItemByInstances(Slot.Value);
		}
	}
}

void UInventoryComponent::AddItemByID(int32 ItemID, int32 AddAmount)
{
	if (CanAddItems(ItemID, AddAmount))
	{
		ServerAddItemByID(ItemID, AddAmount);
	}
}

void UInventoryComponent::RemoveItemByID(int32 ItemID, int32 RemoveAmount,
                                            bool DestroyAfterRemoving)
{
	if (CanRemoveItem(ItemID, RemoveAmount))
	{
		ServerRemoveItemByClass(ItemID, RemoveAmount);
	}
}

void UInventoryComponent::OnRep_InventoryChanged()
{
	OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::CanAddItems(int32 ItemID, int32 Amount)
{
	if (!ItemID || Amount < 1)
	{
		LOGF_INV(Warning, "Invalid input params");
		return false;
	}
	FItemInventorySlot* Slot = FindSlotByID(ItemID);
	if (!Slot)
	{
		if (Amount > InventoryDataSubsystem->GetItemDataByID(ItemID).MaxQuantity)
		{
			LOGF_INV(Warning, "Try to add too much items");
			return false;
		}
		return true;
	}
	if (Slot->GetFreeSpace() < Amount)
	{
		LOGF_INV(Warning, "Not enough space");
		return false;
	}
	return true;
}

void UInventoryComponent::ServerAddItemByInstances_Implementation(const TArray<AMainItemActor*>& Items)
{
	const int32 AddAmount = Items.Num();
	int32 ItemID = Items[0]->ItemID;

	FItemInventorySlot* Slot = FindSlotByID(ItemID);

	if (Slot)
	{
		//add item to slot if it has enough spase
		Slot->Quantity = Slot->Quantity + AddAmount;
		InventorySlots.MarkItemDirty(*Slot);
		OnInventorySlotChanged.Broadcast(*Slot, false);
		for (const auto& Item : Items)
		{
			Item->MulticastDestroyItem();
			LOGF_INV(Display, "Item added to old slot");
		}
	}
	else
	{
		//create new slot
		FItemInventorySlot NewSlot;
		NewSlot.ItemID = ItemID;
		NewSlot.Quantity = AddAmount;
		NewSlot.MaxQuantity = InventoryDataSubsystem->GetItemDataByID(ItemID).MaxQuantity;
		InventorySlots.Items.Add(NewSlot);
		InventorySlots.MarkItemDirty(NewSlot);
		InventorySlots.MarkArrayDirty();
		OnInventorySlotAdded.Broadcast(NewSlot, false);
		LOGF_INV(Display, "New slot is created");
		for (const auto& Item : Items)
		{
			Item->MulticastDestroyItem();
		}
	}
}

bool UInventoryComponent::ServerAddItemByInstances_Validate(const TArray<AMainItemActor*>& Items)
{
	return !Items.IsEmpty() && CanAddItems(Items[0]->ItemID, Items.Num());
}

void UInventoryComponent::ServerAddItemByID_Implementation(int32 ItemID,
                                                           const int32 AddAmount)
{
	if (!CanAddItems(ItemID, AddAmount)) return;

	FItemInventorySlot* Slot = FindSlotByID(ItemID);

	if (Slot)
	{
		//add item to slot if it has enough spase
		Slot->Quantity = Slot->Quantity + AddAmount;
		InventorySlots.MarkItemDirty(*Slot);
		OnInventorySlotChanged.Broadcast(*Slot, false);
		LOGF_INV(Display, "Item added to old slot");
	}
	else
	{
		//create new slot
		FItemInventorySlot NewSlot;
		NewSlot.ItemID = ItemID;
		NewSlot.Quantity = AddAmount;
		NewSlot.MaxQuantity = InventoryDataSubsystem->GetItemDataByID(ItemID).MaxQuantity;
		InventorySlots.Items.Add(NewSlot);
		InventorySlots.MarkItemDirty(NewSlot);
		InventorySlots.MarkArrayDirty();
		OnInventorySlotAdded.Broadcast(NewSlot, false);
		LOGF_INV(Display, "New slot is created");
	}
}

bool UInventoryComponent::ServerAddItemByID_Validate(int32 ItemID,
                                                     const int32 AddAmount)
{
	return CanAddItems(ItemID, AddAmount);
}

bool UInventoryComponent::CanRemoveItem(int32 ItemID, const int32 RemoveAmount)
{
	if (!ItemID || RemoveAmount < 1)
	{
		LOGF_INV(Warning, "Invalid input params");
		return false;
	}
	FItemInventorySlot* Slot = FindSlotByID(ItemID);
	if (!Slot)
	{
		LOGF_INV(Warning, "Can't find slot");
		return false;
	}
	if (Slot->Quantity < RemoveAmount)
	{
		LOGF_INV(Warning, "Not enough items");
		return false;
	}
	return true;
}

void UInventoryComponent::ServerRemoveItemByClass_Implementation(int32 ItemID,
                                                                 const int32 RemoveAmount,
                                                                 bool DestroyAfterRemoving)
{
	FItemInventorySlot* Slot = FindSlotByID(ItemID);

	if (Slot->Quantity == RemoveAmount)
	{
		InventorySlots.MarkItemDirty(*Slot);
		InventorySlots.Items.Remove(*Slot);
		InventorySlots.MarkArrayDirty();
		OnInventorySlotRemoved.Broadcast(*Slot, false);
		LOGF_INV(Display, "Slot is removed");
	}
	else
	{
		Slot->Quantity = Slot->Quantity - RemoveAmount;
		InventorySlots.MarkItemDirty(*Slot);
		OnInventorySlotChanged.Broadcast(*Slot, false);
		LOGF_INV(Display, "Removed %d items from slot", RemoveAmount);
	}
}

bool UInventoryComponent::ServerRemoveItemByClass_Validate(int32 ItemID,
                                                           const int32 RemoveAmount,
                                                           bool DestroyAfterRemoving)
{
	return CanRemoveItem(ItemID, RemoveAmount);
}
