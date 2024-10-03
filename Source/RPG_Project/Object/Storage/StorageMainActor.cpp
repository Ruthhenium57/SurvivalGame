// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageMainActor.h"
#include "../PlayableCharacter.h"
#include "Net/UnrealNetwork.h"

AStorageMainActor::AStorageMainActor()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InteractTextBlockName = "Take";
	InteractTextBlockName2 = "Put";
	MaxQuantity = 0;
}

void AStorageMainActor::HandleInteract(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		std::pair<FItemInventorySlot&, bool> Result = PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass);
		if (Result.second)
		{
			if (!Result.first.Items.IsEmpty())
			{
				if (AMainItemActor* Item = Result.first.Items.Last())
				{
					UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
					if (ItemDataTable)
					{
						FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
						FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
						if (ItemData)
						{
							if (PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass).first.Items.Num() < ItemData->MaxQuantity)
							{
								PlayableCharacter->InventoryComponent->AddItem(Item);
								InventoryComponent->RemoveItem(Item);
								Item->SetOwner(PlayableCharacter);
								UE_LOG(LogTemp, Display, TEXT("Item was taken from the storage"));
							}
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Storage is empty"));
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
		std::pair<FItemInventorySlot&, bool> Result = PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass);
		if (Result.second)
		{
			if (!Result.first.Items.IsEmpty())
			{
				if (AMainItemActor* Item = Result.first.Items.Last())
				{
					UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
					if (ItemDataTable)
					{
						FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
						FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
						if (ItemData)
						{
							if (InventoryComponent->FindSlotByClass(StorageItemClass).first.Items.Num() < MaxQuantity)
							{
								PlayableCharacter->InventoryComponent->RemoveItem(Item);
								InventoryComponent->AddItem(Item);
								Item->SetOwner(this);
								UE_LOG(LogTemp, Display, TEXT("Item has been moved to the storage. Item: %s Num of player items: %d"), *Item->GetName(), Result.first.Items.Num());
							}
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Storage is overflowing"));
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