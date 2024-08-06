// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../InteractableInterface.h"
#include "ObjectMainActor.generated.h"

UCLASS()
class RPG_PROJECT_API AObjectMainActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectMainActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMesh;

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(ACharacter* Character) override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(ACharacter* Character);

	virtual void HandleInteract(ACharacter* Character);

public:

};
