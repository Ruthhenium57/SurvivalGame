// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItemActor.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, bool, bSuccess, AMainItemActor*, Item, int32, Quantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, bool, bSuccess, AMainItemActor*, Item, int32, Quantity);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<AMainItemActor*> Items;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(AMainItemActor* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(AMainItemActor* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(AMainItemActor* Item, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemoved OnItemRemoved;

private:
	UFUNCTION()
	void OnRep_Inventory();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(AMainItemActor* Item, int32 Quantity);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(AMainItemActor* Item, int32 Quantity);

	bool AddItemInternal(AMainItemActor* Item, int32 Quantity);

	bool RemoveItemInternal(AMainItemActor* Item, int32 Quantity);
};
