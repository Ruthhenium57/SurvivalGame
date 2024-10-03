// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MainItemActor.h"
#include "../UI/MainHUDWidget.h"
#include "../ItemDataManager.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedDelegate, bool, bSuccess, AMainItemActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedDelegate, bool, bSuccess, AMainItemActor*, Item);

USTRUCT(BlueprintType)

struct FItemInventorySlot
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMainItemActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AMainItemActor*> Items;

	bool operator==(const FItemInventorySlot& Slot) const
	{
		return ItemClass == Slot.ItemClass && Items == Slot.Items;
	}
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UMainHUDWidget* PlayerWidget;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory, EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
	TArray<FItemInventorySlot> ItemsSlots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(AMainItemActor* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(AMainItemActor* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LogInventory() const;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnItemAddedDelegate OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnItemRemovedDelegate OnItemRemoved;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass, FItemInventorySlot& OutItemSlot);

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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateSlotWidget(FItemInventorySlot ItemSlot);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveSlotWidget(TSubclassOf<AMainItemActor> ItemClass);
};
