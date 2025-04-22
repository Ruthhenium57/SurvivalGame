// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/ItemData.h"
#include "Inventory/MainItemActor.h"
#include "Inventory/InventoryStruct.h"
#include "InventoryComponent.generated.h"

class UInventoryDataSubsystem;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotAdded, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotChanged, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotRemoved, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemActionRejected, TSubclassOf<AMainItemActor>);
DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEMPLUGIN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UInventoryComponent();

	FOnInventorySlotAdded OnInventorySlotAdded;
	FOnInventorySlotChanged OnInventorySlotChanged;
	FOnInventorySlotRemoved OnInventorySlotRemoved;
	FOnItemActionRejected OnItemActionRejected;
	FOnInventoryChanged OnInventoryChanged;
	
	UFUNCTION(BlueprintCallable)
	bool CanAddItems(TSubclassOf<AMainItemActor> ItemClass, int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	bool CanRemoveItem(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount);
	
	FItemInventorySlot* FindSlotByClass(const TSubclassOf<AMainItemActor>& ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FItemInventorySlot>& GetInventorySlots();

	UFUNCTION(BlueprintCallable)
	void AddItemByInstances(const TArray<AMainItemActor*>& Items);

	UFUNCTION(BlueprintCallable)
	void AddItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 AddAmount);

	UFUNCTION(BlueprintCallable)
	void RemoveItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount, bool DestroyAfterRemoving = false);

	UFUNCTION(BlueprintCallable)
	void ClientPredictAddItem(TSubclassOf<AMainItemActor> ItemClass, int32 AddAmount);

	UFUNCTION(BlueprintCallable)
	void ClientPredictRemoveItem(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount);
	
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItemByInstances(const TArray<AMainItemActor*>& Items);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 AddAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItemByClass(TSubclassOf<AMainItemActor> ItemClass, int32 RemoveAmount, bool DestroyAfterRemoving = false);

	UPROPERTY(ReplicatedUsing = OnRep_InventoryChanged)
	FInventoryList InventorySlots;

	UPROPERTY()
	TObjectPtr<UInventoryDataSubsystem> InventorySubsystem;
	
	UFUNCTION()
	void OnRep_InventoryChanged();
};
