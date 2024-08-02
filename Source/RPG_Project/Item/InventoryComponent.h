// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItemActor.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, bool, bSuccess, AMainItemActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, bool, bSuccess, AMainItemActor*, Item);

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
	UPROPERTY(ReplicatedUsing = OnRep_Inventory, EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
	TArray<AMainItemActor*> Items;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(AMainItemActor* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(AMainItemActor* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(AMainItemActor* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventoryByClass(AMainItemActor* ItemClass) const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemoved OnItemRemoved;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<AMainItemActor*> FindAllItemsByClass(AMainItemActor* ItemClass) const;

private:
	UFUNCTION()
	void OnRep_Inventory();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(AMainItemActor* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(AMainItemActor* Item);

	bool AddItemInternal(AMainItemActor* Item);

	bool RemoveItemInternal(AMainItemActor* Item);
};
