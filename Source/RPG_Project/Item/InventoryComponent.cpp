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
		bool B = AddItemInternal(Item);
		OnItemAdded.Broadcast(B, Item);
		return B;
	}
	else
	{
		ServerAddItem(Item);
		UE_LOG(LogTemp, Display, TEXT("Server logiccccccc"));
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
	return !FindAllItemsByClass(Item->GetClass()).IsEmpty();
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

void UInventoryComponent::LogInventoryByClass(TSubclassOf<AMainItemActor> ItemClass) const
{
	int32 Quantity = FindAllItemsByClass(ItemClass).Num();
	UE_LOG(LogTemp, Warning, TEXT("Owner: %s, Item: %s, Quantity: %d"), *GetOwner()->GetName(), *ItemClass->GetName(), Quantity);
}

TArray<AMainItemActor*> UInventoryComponent::FindAllItemsByClass(TSubclassOf<AMainItemActor> ItemClass) const
{
	TArray<AMainItemActor*> FoudedItems;
	for (AMainItemActor* Item : Items)
	{
		if (Item->GetClass() == ItemClass)
		{
			FoudedItems.Add(Item);
		}
	}
	return FoudedItems;
}

void UInventoryComponent::OnRep_Inventory()
{
	
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item)
{
	OnItemAdded.Broadcast(AddItemInternal(Item), Item);
	UE_LOG(LogTemp, Warning, TEXT("ServerAddItemIsCalled"));
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item)
{
	OnItemRemoved.Broadcast(RemoveItemInternal(Item), Item);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item)
{
	return true;
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item)
{
	if (Item)
	{
		int32 TotalQuantity = FindAllItemsByClass(Item->GetClass()).Num();
		if (FindAllItemsByClass(Item->GetClass()).Num() < Item->MaxStack)
		{
			Items.Add(Item);
			LogInventoryByClass(Item->GetClass());
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