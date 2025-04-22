// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftData.h"
#include "Components/ActorComponent.h"
#include "CraftComponent.generated.h"

class UInventoryComponent;

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

	UCraftComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Craft")
	FCraftData GetCraftItemData(const TSubclassOf<AMainItemActor>& ItemClass);
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UFUNCTION(BlueprintCallable, Category = "Craft")
	bool bCanCraft(const TSubclassOf<AMainItemActor>& ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Craft")
	void CraftItem(const TSubclassOf<AMainItemActor>& ItemClass);
	
	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, FCraftData> CraftDataCache;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft", meta = (AllowPrivateAccess))
	ECraftType CraftType;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCraftItem(TSubclassOf<AMainItemActor> ItemClass);
	
	UFUNCTION()
	void CacheCraftDT();
	
	UPROPERTY()
	TObjectPtr<UDataTable> CraftDataTable;
};
