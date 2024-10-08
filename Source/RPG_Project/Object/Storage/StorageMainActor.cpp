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
	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	TextRender->SetupAttachment(StaticMesh);
}

void AStorageMainActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStorageMainActor::HandleInteract(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		FItemInventorySlot StorageSlot;
		InventoryComponent->FindSlotByClass(StorageItemClass, StorageSlot);
		if (!StorageSlot.Items.IsEmpty())
		{
			if (AMainItemActor* Item = StorageSlot.Items.Last())
			{
				UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
				if (ItemDataTable)
				{
					FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
					FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
					if (ItemData)
					{
						FItemInventorySlot PlayerSlot;
						if(PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass, PlayerSlot))
						{
							if (PlayerSlot.Items.Num() < ItemData->MaxQuantity)
							{
								InventoryComponent->RemoveItem(Item);
								PlayableCharacter->InventoryComponent->AddItem(Item);
								Item->SetOwner(PlayableCharacter);
								UE_LOG(LogTemp, Display, TEXT("Item was taken from the storage"));
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("Player storage is overflowing"));
							}
						}
						else
						{
							InventoryComponent->RemoveItem(Item);
							PlayableCharacter->InventoryComponent->AddItem(Item);
							Item->SetOwner(PlayableCharacter);
							UE_LOG(LogTemp, Display, TEXT("Item was taken from the storage"));
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Storage is empty"));
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
		FItemInventorySlot PlayerSlot;
		PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass, PlayerSlot);
		UE_LOG(LogTemp, Display, TEXT("Before accessing PlayerSlot.Items. Num of items: %d"), PlayerSlot.Items.Num());
		if (!PlayerSlot.Items.IsEmpty())
		{
			UE_LOG(LogTemp, Display, TEXT("Before accessing PlayerSlot.Items. Num of items: %d"), PlayerSlot.Items.Num());
			if (AMainItemActor* Item = PlayerSlot.Items.Last())
			{
				UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
				if (ItemDataTable)
				{
					FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
					FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
					if (ItemData)
					{
						FItemInventorySlot StorageSlot;
						if(InventoryComponent->FindSlotByClass(StorageItemClass, StorageSlot))
						{
							UE_LOG(LogTemp, Display, TEXT("Before accessing StorageSlot.Items. Num of items: %d"), StorageSlot.Items.Num());
							if (StorageSlot.Items.Num() < MaxQuantity)
							{
								PlayableCharacter->InventoryComponent->RemoveItem(Item);
								InventoryComponent->AddItem(Item);
								Item->SetOwner(this);
								UE_LOG(LogTemp, Display, TEXT("Item has been moved to the storage"));
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("Storage is overflowing"));
							}
						}
						else
						{
							PlayableCharacter->InventoryComponent->RemoveItem(Item);
							InventoryComponent->AddItem(Item);
							Item->SetOwner(this);
							UE_LOG(LogTemp, Display, TEXT("Item has been moved to the storage"));
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player storage is empty"));
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