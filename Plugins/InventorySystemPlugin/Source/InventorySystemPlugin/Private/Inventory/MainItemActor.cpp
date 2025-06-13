// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MainItemActor.h"
#include "InventoryDataSubsystem.h"
#include "Inventory/InventoryComponent.h"

// Sets default values
AMainItemActor::AMainItemActor()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = SkeletalMesh;
	RootComponent = StaticMesh;
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	ItemID = 0;
}

void AMainItemActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMainItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMainItemActor::Interact(UInventoryComponent* InventoryComponent)
{
	if (InventoryComponent)
	{
		InventoryComponent->AddItemByInstances({this});
	}
}

TArray<EInteractType> AMainItemActor::GetInteractTypes()
{
	return {EInteractType::Take};
}

FName AMainItemActor::GetObjectName()
{
	UInventoryDataSubsystem* InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
	return InventorySubsystem->GetItemDataByID(ItemID).ItemName;
}

void AMainItemActor::MulticastHideItem_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	FVector NewLocation(999999, 999999, 999999);
	SetActorLocation(NewLocation, false);
}

void AMainItemActor::MulticastDestroyItem_Implementation()
{
	Destroy();
}