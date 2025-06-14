// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/MainItemActor.h"
#include "Inventory/InventoryStruct.h"
#include "InventoryComponent.generated.h"

class UInventoryDataSubsystem;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotAdded, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotChanged, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotRemoved, const FItemInventorySlot&, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemActionRejected, int32);
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
	bool CanAddItems(int32 ItemID, int32 Amount);
	
	UFUNCTION(BlueprintCallable)
	bool CanRemoveItem(int32 ItemID, int32 RemoveAmount);
	
	FItemInventorySlot* FindSlotByID(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FItemInventorySlot>& GetInventorySlots();

	UFUNCTION(BlueprintCallable)
	void AddItemByInstances(const TArray<AMainItemActor*>& Items);

	UFUNCTION(BlueprintCallable)
	void AddItemByID(int32 ItemID, int32 AddAmount);

	UFUNCTION(BlueprintCallable)
	void RemoveItemByID(int32 ItemID, int32 RemoveAmount, bool DestroyAfterRemoving = false);

	UFUNCTION(BlueprintCallable)
	void ClientPredictAddItem(int32 ItemID, int32 AddAmount);

	UFUNCTION(BlueprintCallable)
	void ClientPredictRemoveItem(int32 ItemID, int32 RemoveAmount);
	
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItemByInstances(const TArray<AMainItemActor*>& Items);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItemByID(int32 ItemID, int32 AddAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItemByClass(int32 ItemID, int32 RemoveAmount, bool DestroyAfterRemoving = false);

	UPROPERTY(ReplicatedUsing = OnRep_InventoryChanged)
	FInventoryList InventorySlots;

	UPROPERTY()
	TObjectPtr<UInventoryDataSubsystem> InventoryDataSubsystem;
	
	UFUNCTION()
	void OnRep_InventoryChanged();
};
