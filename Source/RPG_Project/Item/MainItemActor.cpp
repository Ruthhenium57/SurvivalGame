// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItemActor.h"
#include "Components/SceneComponent.h"

// Sets default values
AMainItemActor::AMainItemActor()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = SkeletalMesh;
	PrimaryActorTick.bCanEverTick = true;
	ItemLogic = CreateDefaultSubobject<UMainItem>(TEXT("ItemLogic"));
}

void AMainItemActor::Interact(ACharacter* Character)
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

// Called when the game starts or when spawned
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