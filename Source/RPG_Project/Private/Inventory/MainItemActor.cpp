// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MainItemActor.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/HUD/MainHUDWidget.h"
#include "Characters/PlayerCharacter.h"

// Sets default values
AMainItemActor::AMainItemActor()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = SkeletalMesh;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AMainItemActor::Interact(ACharacter* Character)
{
	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Display, TEXT("Running on Server"));
		HandleInteract(Character);
	}
	else
	{
		//UE_LOG(LogTemp, Display, TEXT("Running on Client"));
		ServerInteract(Character);
	}
}

void AMainItemActor::ServerInteract_Implementation(ACharacter* Character)
{
	HandleInteract(Character);
}

bool AMainItemActor::ServerInteract_Validate(ACharacter* Character)
{
	return true;
}

void AMainItemActor::MulticastHideItem_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	FVector NewLocation(999999, 999999, 999999);
	SetActorLocation(NewLocation, false);
}

void AMainItemActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMainItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMainItemActor::HandleInteract(ACharacter* Character)
{
	//UE_LOG(LogTemp, Warning, TEXT("AMainItemActor::Interact called by %s"), *Character->GetName());
	if (Character)
	{
		APlayerCharacter* PlayableCharacter = Cast<APlayerCharacter>(Character);
		if (PlayableCharacter)
		{
			if (PlayableCharacter->InventoryComponent->AddItem(this))
			{
				SetOwner(PlayableCharacter);
				MulticastHideItem();
				return;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Item Not Be Added"));
}
