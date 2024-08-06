// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageMainActor.h"
#include "../PlayableCharacter.h"
#include "Net/UnrealNetwork.h"

AStorageMainActor::AStorageMainActor()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InteractTextBlockName = "Take";
	InteractTextBlockName2 = "Put";
}

void AStorageMainActor::HandleInteract(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		if (InventoryComponent->FindAllItemsByClass(StorageItemClass).IsValidIndex(0))
		{
			if (AMainItemActor* Item = InventoryComponent->FindAllItemsByClass(StorageItemClass)[0])
			{
				if (PlayableCharacter->InventoryComponent->FindAllItemsByClass(StorageItemClass).Num() < Item->MaxStack)
				{
					PlayableCharacter->InventoryComponent->Items.Add(Item);
					InventoryComponent->RemoveItem(Item);
				}
			}
		}
	}
}

void AStorageMainActor::PutItemToStorage(ACharacter* Character)
{
	if (HasAuthority())
	{
		HandlePutItemToStorage(Character);
	}
	else
	{
		ServerPutItemToStorage(Character);
	}
}

void AStorageMainActor::HandlePutItemToStorage(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		if (PlayableCharacter->InventoryComponent->FindAllItemsByClass(StorageItemClass).IsValidIndex(0))
		{
			if (AMainItemActor* Item = PlayableCharacter->InventoryComponent->FindAllItemsByClass(StorageItemClass)[0])
			{
				if (InventoryComponent->FindAllItemsByClass(StorageItemClass).Num() < MaxQuantity)
				{
					InventoryComponent->AddItem(Item);
					PlayableCharacter->InventoryComponent->Items.Remove(Item);
				}
			}
		}
	}
}

void AStorageMainActor::ServerPutItemToStorage_Implementation(ACharacter* Character)
{
	HandlePutItemToStorage(Character);
}

bool AStorageMainActor::ServerPutItemToStorage_Validate(ACharacter* Character)
{
	return true;
}