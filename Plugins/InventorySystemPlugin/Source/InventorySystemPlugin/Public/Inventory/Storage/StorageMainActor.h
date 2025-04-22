// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/InteractableInterface.h"
#include "StorageMainActor.generated.h"

class UInventoryDataSubsystem;
class AMainItemActor;
/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API AStorageMainActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AStorageMainActor();
	
protected:
	virtual void BeginPlay() override;

	virtual void Interact(UInventoryComponent* InventoryComponent) override;

	virtual void SecondInteract(UInventoryComponent* InventoryComponent) override;

	virtual TArray<EInteractType> GetInteractTypes() override;

	virtual FName GetObjectName() override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerPutItemToStorage(UInventoryComponent* PlayerInventoryComponent);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerTakeItemFromStorage(UInventoryComponent* PlayerInventoryComponent);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* StorageInventoryComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<AMainItemActor> StorageItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName StorageName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxQuantity;
};
