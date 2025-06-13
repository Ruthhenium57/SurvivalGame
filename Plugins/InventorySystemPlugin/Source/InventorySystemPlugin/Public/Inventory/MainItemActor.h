// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "MainItemActor.generated.h"

UCLASS()
class INVENTORYSYSTEMPLUGIN_API AMainItemActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AMainItemActor();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideItem();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 ItemID;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Interact(UInventoryComponent* InventoryComponent) override;

	virtual TArray<EInteractType> GetInteractTypes() override;

	virtual FName GetObjectName() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	};
