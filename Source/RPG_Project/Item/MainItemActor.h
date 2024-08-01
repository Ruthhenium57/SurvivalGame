// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainItem.h"
#include "../InteractableInterface.h"
#include "MainItemActor.generated.h"

UCLASS()
class RPG_PROJECT_API AMainItemActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AMainItemActor();

	virtual void Interact(ACharacter* Character) override;
	 
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(ACharacter* Character);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Item")
	UMainItem* ItemLogic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void HandleInteract(ACharacter* Character);
	};
