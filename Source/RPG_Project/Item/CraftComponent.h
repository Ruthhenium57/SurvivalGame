// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.h"
#include "../CraftData.h"
#include "CraftComponent.generated.h"

UENUM(BlueprintType)
enum class ECraftType : uint8
{
	Hand			UMETA(DisplayName = "Hand"),
	Workbench		UMETA(DisplayName = "Workbench")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PROJECT_API UCraftComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Craft")
	bool CraftItem(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION()
	FCraftData GetCraftItemData(TSubclassOf<AMainItemActor> ItemClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft")
	ECraftType CraftType;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

private:
	UFUNCTION()
	bool CraftItemInternal(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCraftItem(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION()
	void CacheCraftDT();

	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, FCraftData> CraftDataCache;
	
	UPROPERTY()
	UDataTable* CraftDataTable;
};
