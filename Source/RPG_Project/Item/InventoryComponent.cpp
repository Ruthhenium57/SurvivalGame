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

	DOREPLIFETIME(UInventoryComponent, Items);
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(AMainItemActor* Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return AddItemInternal(Item);
	}
	else
	{
		ServerAddItem(Item);
		return false;
	}
}

bool UInventoryComponent::RemoveItem(AMainItemActor* Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return RemoveItemInternal(Item);
	}
	else
	{
		ServerRemoveItem(Item);
		return false;
	}
}

bool UInventoryComponent::HasItem(AMainItemActor* Item) const
{
	return !FindAllItemsByClass(Item).IsEmpty();
}

void UInventoryComponent::LogInventory() const
{
	for (AMainItemActor* InventoryItem : Items)
	{
		if (InventoryItem)
		{
			UE_LOG(LogTemp, Warning, TEXT("Owner: %s, Item: %s"), *GetOwner()->GetName(), *InventoryItem->GetName());
		}
	}
}

void UInventoryComponent::LogInventoryByClass(AMainItemActor* ItemClass) const
{
	int32 Quantity = FindAllItemsByClass(ItemClass).Num();
	UE_LOG(LogTemp, Warning, TEXT("Item: %s, Quantity: %d"), *ItemClass->ItemName, Quantity);
}

TArray<AMainItemActor*> UInventoryComponent::FindAllItemsByClass(AMainItemActor* ItemClass) const
{
	TArray<AMainItemActor*> FoudedItems;
	for (AMainItemActor* Item : Items)
	{
		if (Item->GetClass() == ItemClass->GetClass())
		{
			FoudedItems.Add(Item);
		}
	}
	return TArray<AMainItemActor*>(FoudedItems);
}

void UInventoryComponent::OnRep_Inventory()
{
	LogInventory();
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item)
{
	//OnItemAdded.Broadcast(AddItemInternal(Item), Item);
	AddItemInternal(Item);
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item)
{
	//OnItemRemoved.Broadcast(RemoveItemInternal(Item), Item);
	RemoveItemInternal(Item);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item)
{
	return true;
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item)
{
	if (Item)
	{
		int32 TotalQuantity = FindAllItemsByClass(Item).Num();
		if (FindAllItemsByClass(Item).Num() < Item->MaxStack)
		{
			Items.Add(Item);
			UE_LOG(LogTemp, Warning, TEXT("Item Added"));
			return true;
		}
		UE_LOG(LogTemp, Error, TEXT("Not enough space"));
	}

	return false;
}

bool UInventoryComponent::RemoveItemInternal(AMainItemActor* Item)
{
	return Items.Remove(Item) > 0;
}