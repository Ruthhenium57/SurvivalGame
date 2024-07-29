// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItem.h"
#include "InventoryComponent.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

private:
	UFUNCTION()
	void OnRep_Inventory();
};
