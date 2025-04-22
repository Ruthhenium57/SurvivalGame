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

	InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, InventorySlots, COND_OwnerOnly);
}

FItemInventorySlot* UInventoryComponent::FindSlotByClass(const TSubclassOf<AMainItemActor>& ItemClass)
{
	if (!ItemClass) return nullptr;

	for (FItemInventorySlot& Slot : InventorySlots.Items)
	{
		if (Slot.ItemClass == ItemClass) return &Slot;
	}

	return nullptr;
}

const TArray<FItemInventorySlot>& UInventoryComponent::GetInventorySlots()
{
	return InventorySlots.Items;
}

void UInventoryComponent::ClientPredictAddItem(TSubclassOf<AMainItemActor> ItemClass, int32 AddAmount)
{
	if (CanAddItems(ItemClass, AddAmount))
	{
		if (!GetOwner()->HasAuthority())
		{
			FItemInventorySlot* Slot = FindSlotByClass(ItemClass);
			if (Slot)
			{
				OnInventorySlotChanged.Broadcast(*Slot, true);
				return;
			}
			FItemInventorySlot NewSlot;
			NewSlot.ItemClass = ItemClass;
			NewSlot.Quantity = AddAmount;
			NewSlot.MaxQuantity = InventorySubsystem->GetItemDataByClass(ItemClass).MaxQuantity;
			OnInventorySlotAdded.Broadcast(NewSlot, true);
		}
	}
}

void UInventoryComponent::ClientPredictRemoveItem(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount)
{
	if (!GetOwner()->HasAuthority())
	{
		FItemInventorySlot* Slot = FindSlotByClass(ItemClass);
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
	TMap<TSubclassOf<AMainItemActor>, TArray<AMainItemActor*>> SortedItems;
	for (AMainItemActor* Item : Items)
	{
		if (!Item) continue;
		TArray<AMainItemActor*>* ExistingItems = SortedItems.Find(Item->GetClass());
		if (ExistingItems)
		{
			ExistingItems->Add(Item);
		}
		else
		{
			TArray<AMainItemActor*> NewArray;
			NewArray.Add(Item);
			SortedItems.Add(Item->GetClass(), NewArray);
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

void UInventoryComponent::AddItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 AddAmount)
{
	if (CanAddItems(ItemClass, AddAmount))
	{
		ServerAddItemByClass(ItemClass, AddAmount);
	}
}

void UInventoryComponent::RemoveItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount,
                                            bool DestroyAfterRemoving)
{
	if (CanRemoveItem(ItemClass, RemoveAmount))
	{
		ServerRemoveItemByClass(ItemClass, RemoveAmount);
	}
}

void UInventoryComponent::OnRep_InventoryChanged()
{
	OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::CanAddItems(TSubclassOf<AMainItemActor> ItemClass, int32 Amount)
{
	if (!ItemClass || Amount < 1)
	{
		LOGF_INV(Warning, "Invalid input params");
		return false;
	}
	FItemInventorySlot* Slot = FindSlotByClass(ItemClass);
	if (!Slot)
	{
		if (Amount > InventorySubsystem->GetItemDataByClass(ItemClass).MaxQuantity)
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
	TSubclassOf<AMainItemActor> ItemClass = Items[0]->GetClass();

	FItemInventorySlot* Slot = FindSlotByClass(ItemClass);

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
		NewSlot.ItemClass = ItemClass;
		NewSlot.Quantity = AddAmount;
		NewSlot.MaxQuantity = InventorySubsystem->GetItemDataByClass(ItemClass).MaxQuantity;
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
	return !Items.IsEmpty() && CanAddItems(Items[0]->GetClass(), Items.Num());
}

void UInventoryComponent::ServerAddItemByClass_Implementation(const TSubclassOf<AMainItemActor> ItemClass,
                                                              const int32 AddAmount)
{
	if (!CanAddItems(ItemClass, AddAmount)) return;

	FItemInventorySlot* Slot = FindSlotByClass(ItemClass);

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
		NewSlot.ItemClass = ItemClass;
		NewSlot.Quantity = AddAmount;
		NewSlot.MaxQuantity = InventorySubsystem->GetItemDataByClass(ItemClass).MaxQuantity;
		InventorySlots.Items.Add(NewSlot);
		InventorySlots.MarkItemDirty(NewSlot);
		InventorySlots.MarkArrayDirty();
		OnInventorySlotAdded.Broadcast(NewSlot, false);
		LOGF_INV(Display, "New slot is created");
	}
}

bool UInventoryComponent::ServerAddItemByClass_Validate(const TSubclassOf<AMainItemActor> ItemClass,
                                                        const int32 AddAmount)
{
	return CanAddItems(ItemClass, AddAmount);
}

bool UInventoryComponent::CanRemoveItem(TSubclassOf<AMainItemActor> ItemClass, const int32 RemoveAmount)
{
	if (!ItemClass || RemoveAmount < 1)
	{
		LOGF_INV(Warning, "Invalid input params");
		return false;
	}
	FItemInventorySlot* Slot = FindSlotByClass(ItemClass);
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

void UInventoryComponent::ServerRemoveItemByClass_Implementation(TSubclassOf<AMainItemActor> ItemClass,
                                                                 const int32 RemoveAmount,
                                                                 bool DestroyAfterRemoving)
{
	FItemInventorySlot* Slot = FindSlotByClass(ItemClass);

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

bool UInventoryComponent::ServerRemoveItemByClass_Validate(TSubclassOf<AMainItemActor> ItemClass,
                                                           const int32 RemoveAmount,
                                                           bool DestroyAfterRemoving)
{
	return CanRemoveItem(ItemClass, RemoveAmount);
}
