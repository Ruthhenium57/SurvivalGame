// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItemActor.h"
#include "../UI/MainHUDWidget.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedDelegate, bool, bSuccess, AMainItemActor*, Item);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedDelegate, bool, bSuccess, AMainItemActor*, Item);

USTRUCT(BlueprintType)

struct FItemInventorySlot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AMainItemActor*> Items;

	/*bool operator==(const FItemInventorySlot& Slot) const
	{
		return ItemClass == Slot.ItemClass && Items == Slot.Items;
	}*/
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
	bool RemoveItem(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	/*UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnItemAddedDelegate OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnItemRemovedDelegate OnItemRemoved;*/

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemInventorySlot FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 HowMuchFreeSpaceInSlot(TSubclassOf<AMainItemActor> ItemClass);

private:
	UFUNCTION()
	void OnRep_Inventory();

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TMap<FString, FItemInventorySlot> InventorySlots;

	/*UPROPERTY(ReplicatedUsing = OnRep_Inventory, EditAnywhere, Replicated, Category = "Inventory")
	TArray<FItemInventorySlot> ItemsSlots;*/

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddItem(AMainItemActor* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoveItem(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION()
	bool AddItemInternal(AMainItemActor* Item);

	UFUNCTION()
	bool RemoveItemInternal(AMainItemActor* Item, TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateSlotWidget(FItemInventorySlot ItemSlot);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveSlotWidget(TSubclassOf<AMainItemActor> ItemClass);
};
