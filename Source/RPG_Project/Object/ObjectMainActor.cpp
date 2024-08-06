// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectMainActor.h"

// Sets default values
AObjectMainActor::AObjectMainActor()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AObjectMainActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void AObjectMainActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectMainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObjectMainActor::Interact(ACharacter* Character)
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

void AObjectMainActor::HandleInteract(ACharacter* Character)
{
	
}

void AObjectMainActor::ServerInteract_Implementation(ACharacter* Character)
{
	HandleInteract(Character);
}

bool AObjectMainActor::ServerInteract_Validate(ACharacter* Character)
{
	return true;
}