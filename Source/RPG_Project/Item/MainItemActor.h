// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void HandleInteract(ACharacter* Character);
	};
