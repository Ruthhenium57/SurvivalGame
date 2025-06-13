// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Storage/StorageMainActor.h"
#include "Inventory/InventoryComponent.h"


AStorageMainActor::AStorageMainActor()
{
	StorageInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AStorageMainActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStorageMainActor::Interact(UInventoryComponent* InventoryComponent)
{
	ServerTakeItemFromStorage(InventoryComponent);
}

void AStorageMainActor::SecondInteract(UInventoryComponent* InventoryComponent)
{
	ServerPutItemToStorage(InventoryComponent);
}

TArray<EInteractType> AStorageMainActor::GetInteractTypes()
{
	return {EInteractType::Take, EInteractType::Put};
}

FName AStorageMainActor::GetObjectName()
{
	return StorageName;
}

void AStorageMainActor::ServerTakeItemFromStorage_Implementation(UInventoryComponent* PlayerInventoryComponent)
{
	if (PlayerInventoryComponent)
	{
		if (StorageInventoryComponent->CanRemoveItem(StorageItemID, 1))
		{
			if (PlayerInventoryComponent->CanAddItems(StorageItemID, 1))
			{
				StorageInventoryComponent->RemoveItemByID(StorageItemID, 1);
				PlayerInventoryComponent->AddItemByID(StorageItemID, 1);
			}
		}
	}
}

bool AStorageMainActor::ServerTakeItemFromStorage_Validate(UInventoryComponent* PlayerInventoryComponent)
{
	return true;
}

void AStorageMainActor::ServerPutItemToStorage_Implementation(UInventoryComponent* PlayerInventoryComponent)
{
	if (PlayerInventoryComponent)
	{
		if (PlayerInventoryComponent->CanRemoveItem(StorageItemID, 1))
		{
			if (StorageInventoryComponent->CanAddItems(StorageItemID, 1))
			{
				StorageInventoryComponent->AddItemByID(StorageItemID, 1);
				PlayerInventoryComponent->RemoveItemByID(StorageItemID, 1);
			}
		}
    }
}

bool AStorageMainActor::ServerPutItemToStorage_Validate(UInventoryComponent* PlayerInventoryComponent)
{
	return true;
}
