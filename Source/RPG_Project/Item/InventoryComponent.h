// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItem.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, bool, bSuccess, UMainItem*, Item, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, bool, bSuccess, UMainItem*, Item, int32, Quantity);

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMainItem* Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	FInventorySlot() : Item(nullptr), Quantity(0) {}
};

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_Inventory, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlot> Inventory;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UMainItem* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UMainItem* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(UMainItem* Item, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemoved OnItemRemoved;

private:
	UFUNCTION()
	void OnRep_Inventory();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(UMainItem* Item, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(UMainItem* Item, int32 Quantity);

private:
	bool AddItemInternal(UMainItem* Item, int32 Quantity);

	bool RemoveItemInternal(UMainItem* Item, int32 Quantity);
};
