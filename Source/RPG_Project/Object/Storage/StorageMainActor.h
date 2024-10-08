// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ObjectMainActor.h"
#include "../Item/InventoryComponent.h"
#include "Components/TextRenderComponent.h"
#include "StorageMainActor.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AStorageMainActor : public AObjectMainActor
{
	GENERATED_BODY()
	
public:
	AStorageMainActor();

	virtual void BeginPlay() override;
	void HandleInteract(ACharacter* Character) override;

	void PutItemToStorage(ACharacter* Character) override;
	void HandlePutItemToStorage(ACharacter* Character);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPutItemToStorage(ACharacter* Character);

	UPROPERTY(EditAnywhere, Category = "Components")
	UTextRenderComponent* TextRender;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<class AMainItemActor> StorageItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxQuantity;
};
