// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItemActor.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "../PlayableCharacter.h"

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

void AMainItemActor::BeginPlay()
{
	Super::BeginPlay();
	UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_ItemData.DT_ItemData"));
	UE_LOG(LogTemp, Display, TEXT("Huechek"));
	if (ItemDataTable)
	{
		FString ContextString = "";
		FName RowName = FName(TEXT("Stone"));
		FItemData* ItemData = ItemDataTable->FindRow<FItemData>(RowName, ContextString);
		if (ItemData)
		{
			InteractTextBlockName = ItemData->InteractTextBlockName;
			InteractTextBlockName2 = ItemData->InteractTextBlockName2;
			UE_LOG(LogTemp, Display, TEXT("Hui"));
		}
	}//GetClass()->GetName().RightChop(7).LeftChop(2)
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
		APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
		if (PlayableCharacter)
		{
			if (PlayableCharacter->InventoryComponent->AddItem(this))
			{
				FVector NewLocation(9999, 9999, 9999);
				SetActorLocation(NewLocation);
				SetActorHiddenInGame(true);
				SetActorEnableCollision(false);
				SetOwner(PlayableCharacter);
				return;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Item Not Be Added"));
}
