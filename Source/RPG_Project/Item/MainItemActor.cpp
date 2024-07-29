// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItemActor.h"

// Sets default values
AMainItemActor::AMainItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemLogic = CreateDefaultSubobject<UMainItem>(TEXT("ItemLogic"));
}

void AMainItemActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Interacted"));
	
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