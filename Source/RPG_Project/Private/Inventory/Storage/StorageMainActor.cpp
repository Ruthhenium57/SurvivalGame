// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Storage/StorageMainActor.h"
#include "Characters/PlayerCharacter.h"
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

void AStorageMainActor::OnConstruction(const FTransform& Transform)
{
	
}

void AStorageMainActor::BeginPlay()
{
	Super::BeginPlay();
}

void AStorageMainActor::Tick(float DeltaTime)
{
	
}

void AStorageMainActor::HandleInteract(ACharacter* Character)
{
	APlayerCharacter* PlayableCharacter = Cast<APlayerCharacter>(Character);
	if (PlayableCharacter)
	{
		FItemInventorySlot StorageSlot = InventoryComponent->FindSlotByClass(StorageItemClass);
		if (!StorageSlot.Items.IsEmpty())
		{
			if (AMainItemActor* Item = StorageSlot.Items.Last())
			{
				FItemInventorySlot PlayerSlot = PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass);
				if(!PlayerSlot.Items.IsEmpty())
				{
					if (PlayerSlot.Items.Num() < PlayableCharacter->InventoryComponent->GetItemDataFromTable(StorageItemClass).MaxQuantity)
					{
						InventoryComponent->RemoveItem(Item);
						PlayableCharacter->InventoryComponent->AddItem(Item);
						Item->SetOwner(PlayableCharacter);
					}
					else UE_LOG(LogTemp, Error, TEXT("StorageActor: Player storage is overflowing"));
				}
				else
				{
					InventoryComponent->RemoveItem(Item);
					PlayableCharacter->InventoryComponent->AddItem(Item);
					Item->SetOwner(PlayableCharacter);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("StorageActor: Storage is empty"));
		}
	}
}

void AStorageMainActor::PutItemToStorage(ACharacter* Character)
{
	if (HasAuthority())
	{
		PutItemToStorageInternal(Character);
	}
	else
	{
		ServerPutItemToStorage(Character);
	}
}

void AStorageMainActor::PutItemToStorageInternal(ACharacter* Character)
{
	APlayerCharacter* PlayableCharacter = Cast<APlayerCharacter>(Character);
	if (PlayableCharacter)
	{
		FItemInventorySlot PlayerSlot = PlayableCharacter->InventoryComponent->FindSlotByClass(StorageItemClass);
		if (!PlayerSlot.Items.IsEmpty())
		{
			if (AMainItemActor* Item = PlayerSlot.Items.Last())
			{
				FItemInventorySlot StorageSlot = InventoryComponent->FindSlotByClass(StorageItemClass);
				if(!StorageSlot.Items.IsEmpty())
				{
					if (StorageSlot.Items.Num() < MaxQuantity)
					{
						PlayableCharacter->InventoryComponent->RemoveItem(Item);
						InventoryComponent->AddItem(Item);
						Item->SetOwner(this);
					}
					else UE_LOG(LogTemp, Error, TEXT("StorageActor: Storage is overflowing"));
				}
				else
				{
					PlayableCharacter->InventoryComponent->RemoveItem(Item);
					InventoryComponent->AddItem(Item);
					Item->SetOwner(this);
				}
			}
		}
		else UE_LOG(LogTemp, Error, TEXT("StorageActor: Player storage is empty"));
	}
}

void AStorageMainActor::ServerPutItemToStorage_Implementation(ACharacter* Character)
{
	PutItemToStorageInternal(Character);
}

bool AStorageMainActor::ServerPutItemToStorage_Validate(ACharacter* Character)
{
	return true;
}