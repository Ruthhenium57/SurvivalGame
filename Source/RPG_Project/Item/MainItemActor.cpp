// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItemActor.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMainItemActor::AMainItemActor()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = SkeletalMesh;
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ItemLogic = CreateDefaultSubobject<UMainItem>(TEXT("ItemLogic"));
}

void AMainItemActor::Interact(ACharacter* Character)
{
	if (HasAuthority())
	{
		HandleInteract(Character);
	}
	else
	{
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
	if (ItemLogic == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemLogic is nullptr in AMainItemActor::BeginPlay"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemLogic successfully initialized in AMainItemActor::BeginPlay"));
	}
}

// Called every frame
void AMainItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainItemActor, ItemLogic);
}

void AMainItemActor::HandleInteract(ACharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("AMainItemActor::Interact called by %s"), *Character->GetName());
	if (Character && ItemLogic)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Interacted"));
		if (ItemLogic->Interact(Character))
		{
			FVector NewLocation(9999, 9999, 9999);
			SetActorLocation(NewLocation);
			SetActorHiddenInGame(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Item Not Be Added"));
		}
	}
	if (ItemLogic == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemLogicIsNullptr"));
	}
}
