// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "InvenroryWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
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
			MulticastUpdateSlotWidget(*InventorySlots.Find(Item->GetClass()->GetName()));
		}
		return bSuccess;
	}
	else
	{
		ServerAddItem(Item);
		return false;
	}
}

bool UInventoryComponent::RemoveItem(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		bool bSuccess = RemoveItemInternal(Item, ItemClass);
		if (bSuccess)
		{
			MulticastUpdateSlotWidget(*InventorySlots.Find(ItemClass->GetName()));
		}
		return bSuccess;
	}
	else
	{
		ServerRemoveItem(Item, ItemClass);
		return false;
	}
}

void UInventoryComponent::LogInventory() const
{
	
}

FItemInventorySlot UInventoryComponent::FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass)
{
	FItemInventorySlot* Slot = InventorySlots.Find(ItemClass->GetName());
	return *Slot;
}

int32 UInventoryComponent::HowMuchFreeSpaceInSlot(TSubclassOf<AMainItemActor> ItemClass)
{
	FItemData* ItemData = nullptr;
	UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item")); // get data table

	if (ItemDataTable)
	{
		ItemData = ItemDataTable->FindRow<FItemData>(FName(ItemClass->GetName().RightChop(7).LeftChop(2)), TEXT("")); // find row

		if (ItemData)
		{
			FItemInventorySlot* Slot = InventorySlots.Find(ItemClass->GetName()); // find slot
			if (Slot)
			{
				return ItemData->MaxQuantity - Slot->Items.Num(); // return free space
			}
			else
			{
				return ItemData->MaxQuantity; // return max quantity
			}
		}
	}
}

void UInventoryComponent::OnRep_Inventory()
{
	
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item)
{
	AddItem(Item);
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass)
{
	RemoveItemInternal(Item, ItemClass);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass)
{
	return true;
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item)
{
	if (Item)
	{
		FItemData* ItemData = nullptr;
		UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
		if (ItemDataTable)
		{
			FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
			ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
		}

		if (ItemData)
		{
			if (FItemInventorySlot* Slot = InventorySlots.Find(Item->GetClass()->GetName())) // If slot is available
			{
				if (Slot->Items.Num() < ItemData->MaxQuantity) // if has enough space
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
			InventorySlots.Add(Item->GetClass()->GetName(), Slot); // add new element to map
			UE_LOG(LogTemp, Display, TEXT("New slot is created"));
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveItemInternal(AMainItemActor* Item = nullptr, TSubclassOf<AMainItemActor> ItemClass = nullptr)
{
	if (Item)
	{
		ItemClass = Item->GetClass();
	}

	if (!ItemClass)
	{
		return false; // return if no parameters were specified
	}

	if (FItemInventorySlot* Slot = InventorySlots.Find(ItemClass->GetName()))
	{
		if (Slot->Items.Num() == 1) // If this last item in slot
		{
			InventorySlots.Remove(ItemClass->GetName()); // remove slot
			UE_LOG(LogTemp, Display, TEXT("Slot is removed"));
			return true;
		}
		else
		{
			if (Item)
			{
				Slot->Items.Remove(Item); // remove item from slot
				UE_LOG(LogTemp, Display, TEXT("Item removed from slot"));
				return true;
			}
			else
			{
				Slot->Items.Remove(Slot->Items.Last()); // remove last item from slot
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Item not been removed"));
	return false;
}

void UInventoryComponent::MulticastUpdateSlotWidget_Implementation(FItemInventorySlot ItemSlot)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled())
		{
			if (PlayerWidget)
			{
				PlayerWidget->InventoryWidget->UpdateSlotInfo(ItemSlot); // update inventory widget
				UE_LOG(LogTemp, Warning, TEXT("Inventory updated on local client"));
			}
		}
	}
}

void UInventoryComponent::MulticastRemoveSlotWidget_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{

}
