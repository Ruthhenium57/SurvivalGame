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

	DOREPLIFETIME(UInventoryComponent, Inventory)
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(UMainItem* Item, int32 Quantity)
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

bool UInventoryComponent::RemoveItem(UMainItem* Item, int32 Quantity)
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

bool UInventoryComponent::HasItem(UMainItem* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	for (const FInventorySlot& Slot : Inventory)
	{
		if (Slot.Item == Item && Slot.Quantity >= Quantity)
		{
			return true;
		}
	}

	return false;
}

void UInventoryComponent::LogInventory() const
{
	for (const FInventorySlot& Slot : Inventory)
	{
		if (Slot.Item)
		{
			UE_LOG(LogTemp, Display, TEXT("Item: %s, Quantity: %d"), *Slot.Item->Name, Slot.Quantity);
		}
	}
}

void UInventoryComponent::OnRep_Inventory()
{
	LogInventory();
}