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

void AMainItemActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Interacted"));
	ItemLogic->Interact();
}

// Called when the game starts or when spawned
void AMainItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}