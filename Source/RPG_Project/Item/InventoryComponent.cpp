// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "InventoryWidget.h"
#include "Engine/DataTable.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheItemDataTable();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UInventoryComponent, InventorySlots);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(AMainItemActor* Item)
{
	if (GetOwner()->HasAuthority())
	{
		bool bSuccess = AddItemInternal(Item);
		if (bSuccess)
		{
			MulticastUpdateSlotWidget(Item->GetClass());
		}
		return bSuccess;
	}
	else
	{
		ServerAddItem(Item);
		return false;
	}
}

bool UInventoryComponent::RemoveItem(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass, bool DestroyAfretRemoving)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		bool bSuccess = RemoveItemInternal(Item, ItemClass, DestroyAfretRemoving);
		if (bSuccess)
		{
			if (Item)
			{
				ItemClass = Item->GetClass();
			}
			MulticastUpdateSlotWidget(ItemClass);
		}
		return bSuccess;
	}
	else
	{
		ServerRemoveItem(Item, ItemClass, DestroyAfretRemoving);
		return false;
	}
}

void UInventoryComponent::LogInventory() const
{
	
}

FItemInventorySlot UInventoryComponent::FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass)
{
	FItemInventorySlot* Slot = InventorySlots.Find(ItemClass);
	return Slot ? *Slot : FItemInventorySlot();
}

int32 UInventoryComponent::HowMuchFreeSpaceInSlot(TSubclassOf<AMainItemActor> ItemClass)
{
	if (!ItemClass) return 0; // ItemClass is empty
	FItemInventorySlot* Slot = InventorySlots.Find(ItemClass); // find slot
	if (Slot) return GetItemDataFromTable(ItemClass).MaxQuantity - Slot->Items.Num(); // return free space
	else return GetItemDataFromTable(ItemClass).MaxQuantity; // return max quantity
}

const TMap<TSubclassOf<AMainItemActor>, FItemInventorySlot> UInventoryComponent::GetInventoryItems()
{
	return InventorySlots;
}

void UInventoryComponent::MulticastUpdateSpecificSlots_Implementation(const TArray<TSubclassOf<AMainItemActor>>& ChangedKeys)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled() && PlayerWidget)
		{
			for (const TSubclassOf<AMainItemActor> Key : ChangedKeys)
			{
				PlayerWidget->InventoryWidget->UpdateSlotInfo(Key);
			}
		}
	}
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item)
{
	AddItem(Item);
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass, bool DestroyAfretRemoving)
{
	RemoveItemInternal(Item, ItemClass, DestroyAfretRemoving);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass, bool DestroyAfretRemoving)
{
	return true;
}

FItemData UInventoryComponent::GetItemDataFromTable(TSubclassOf<AMainItemActor> ItemClass)
{
	// find data from cache
	if (!ItemClass) return FItemData();
	const FItemData* ItemData = ItemDataCache.Find(ItemClass);
	if (ItemData) return *ItemData;
	UE_LOG(LogTemp, Error, TEXT("InventoryComp::GetItemDataFromTable: No item data found for class %s"), *ItemClass->GetName());
	return FItemData();
}

void UInventoryComponent::CacheItemDataTable()
{
	if (!ItemDataTable)		
	{	// load item  DT
		ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
		if (!ItemDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComp::CachingDT: Failed to load DT"));
			return;
		}
	}
	// create cache
	for (const TPair<FName, uint8*>& Row : ItemDataTable->GetRowMap())
	{
		const FItemData* ItemData = reinterpret_cast<const FItemData*>(Row.Value);
		if (ItemData && ItemData->ItemClass) {
			ItemDataCache.Add(ItemData->ItemClass, *ItemData);
		}
	}
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item)
{
	if (!Item) return false;
	if (FItemInventorySlot* Slot = InventorySlots.Find(Item->GetClass())) // If slot is available
	{
		if (Slot->Items.Num() < GetItemDataFromTable(Item->GetClass()).MaxQuantity) // if has enough space
		{
			Slot->Items.Add(Item);						// add item
			UE_LOG(LogTemp, Display, TEXT("ItemAddedToOldSlot"));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Not enough space"));
			return false;
		}
	}
	FItemInventorySlot Slot;
	Slot.Items.Add(Item);
	InventorySlots.Add(Item->GetClass(), Slot); // add new element to map
	UE_LOG(LogTemp, Display, TEXT("New slot is created"));
	return true;
}

bool UInventoryComponent::RemoveItemInternal(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass, bool DestroyAfretRemoving)
{
	if (Item) ItemClass = Item->GetClass();

	if (!ItemClass) return false; // return if no parameters were specified

	if (FItemInventorySlot* Slot = InventorySlots.Find(ItemClass))
	{
		if (Slot->Items.Num() == 1) // If this last item in slot
		{
			if (DestroyAfretRemoving) { FindSlotByClass(ItemClass).Items.Last()->Destroy(); } // if u need destroy item
			InventorySlots.Remove(ItemClass);					// remove slot
			UE_LOG(LogTemp, Display, TEXT("Slot is removed"));
			return true;
		}
		else
		{
			if (Item)
			{		// remove item from slot
				Slot->Items.Remove(Item); 
				if (DestroyAfretRemoving) { Item->Destroy(); }
				UE_LOG(LogTemp, Display, TEXT("Item removed from slot"));
				return true;
			}
			else
			{		// remove last item from slot
				AMainItemActor* ItemByClass = Slot->Items.Last();
				Slot->Items.Remove(ItemByClass); 
				if (DestroyAfretRemoving) { ItemByClass->Destroy(); }
				UE_LOG(LogTemp, Display, TEXT("Item removed from slot"));
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Item not been removed"));
	return false;
}

void UInventoryComponent::MulticastUpdateSlotWidget_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled())
		{
			if (PlayerWidget)
			{
				PlayerWidget->InventoryWidget->UpdateSlotInfo(ItemClass); // update inventory widget
				UE_LOG(LogTemp, Warning, TEXT("Inventory updated on local client"));
			}
		}
	}
}

void UInventoryComponent::MulticastRemoveSlotWidget_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{

}
