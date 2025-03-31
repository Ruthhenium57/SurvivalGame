// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItemActor.h"
#include "../UI/MainHUDWidget.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

USTRUCT(BlueprintType)

struct FItemInventorySlot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AMainItemActor*> Items;
};

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UMainHUDWidget* PlayerWidget;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(AMainItemActor* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(AMainItemActor* Item = nullptr, TSubclassOf<AMainItemActor> ItemClass = nullptr, bool DestroyAfretRemoving = false);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemInventorySlot FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 HowMuchFreeSpaceInSlot(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TMap<TSubclassOf<AMainItemActor>, FItemInventorySlot> GetInventoryItems();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateSlotWidget(TSubclassOf<AMainItemActor> ItemClass);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateSpecificSlots(const TArray<TSubclassOf<AMainItemActor>>& ChangedKeys);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UDataTable* ItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemData GetItemDataFromTable(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CacheItemDataTable();

private:
	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, FItemInventorySlot> InventorySlots;

	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, FItemData> ItemDataCache;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(AMainItemActor* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass, bool DestroyAfretRemoving);

	UFUNCTION()
	bool AddItemInternal(AMainItemActor* Item);

	UFUNCTION()
	bool RemoveItemInternal(AMainItemActor* Item = nullptr, TSubclassOf<AMainItemActor> ItemClass = nullptr, bool DestroyAfretRemoving = false);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveSlotWidget(TSubclassOf<AMainItemActor> ItemClass);
};
