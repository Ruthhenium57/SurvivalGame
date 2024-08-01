// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

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

	DOREPLIFETIME(AMainItemActor*, Items);
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(AMainItemActor* Item, int32 Quantity)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return AddItemInternal(Item, Quantity);
	}
	else
	{
		ServerAddItem(Item, Quantity);
		return false;
	}
}

bool UInventoryComponent::RemoveItem(AMainItemActor* Item, int32 Quantity)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return RemoveItemInternal(Item, Quantity);
	}
	else
	{
		ServerRemoveItem(Item, Quantity);
		return false;
	}
}

bool UInventoryComponent::HasItem(AMainItemActor* Item, int32 Quantity) const
{
	

	return false;
}

void UInventoryComponent::LogInventory() const
{
	//for (const Items)
	//{
	//	if (Slot.Item)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Owner: %s, Item: %s, Quantity: %d"), *GetOwner()->GetName(), *Slot.Item->GetName(), Slot.Quantity);
	//	}
	//}
}

void UInventoryComponent::OnRep_Inventory()
{
	LogInventory();
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item, int32 Quantity)
{
	OnItemAdded.Broadcast(AddItemInternal(Item, Quantity), Item, Quantity);
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item, int32 Quantity)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item, int32 Quantity)
{
	OnItemRemoved.Broadcast(RemoveItemInternal(Item, Quantity), Item, Quantity);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item, int32 Quantity)
{
	return true;
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}
	
	for (FInventorySlot& Slot : Inventory)
	{
		if (Slot.Item == Item)
		{
			int32 SpaceLeft = Item->MaxQuantity - Slot.Quantity;
			if (SpaceLeft > 0)
			{
				int32 ToAdd = FMath::Min(SpaceLeft, Quantity);
				Slot.Quantity += ToAdd;
				Quantity -= ToAdd;

				if (Quantity <= 0)
				{
					OnRep_Inventory();
					return true;
				}
				else
				{
					OnRep_Inventory();
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	if (Quantity > 0)
	{
		FInventorySlot NewSlot;
		NewSlot.Item = Item;
		NewSlot.Quantity = FMath::Min(Item->MaxQuantity, Quantity);
		Inventory.Add(NewSlot);
		Quantity -= NewSlot.Quantity;

		OnRep_Inventory();
		return Quantity <= 0;
	}

	return false;
}

bool UInventoryComponent::RemoveItemInternal(AMainItemActor* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	for (int32 i = Inventory.Num() - 1; i >= 0; --i)
	{
		FInventorySlot& Slot = Inventory[i];
		if (Slot.Item == Item)
		{
			if (Slot.Quantity >= Quantity)
			{
				Slot.Quantity -= Quantity;

				if (Slot.Quantity == 0)
				{
					Inventory.RemoveAt(i);
				}

				OnRep_Inventory();
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}