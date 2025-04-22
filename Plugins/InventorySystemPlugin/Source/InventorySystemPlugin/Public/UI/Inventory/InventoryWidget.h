// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "InventoryWidget.generated.h"

class AMainItemActor;
class UInventoryComponent;
struct FItemInventorySlot;
class UScrollBox;
class UItemSlotWidget;
class AMainPlayerState;

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	void InitializeAll();
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRefreshInfo(const FItemInventorySlot& ItemSlot);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnUpdateInventory(const TArray<FItemInventorySlot>& ItemSlots);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnAddNewSlot(const FItemInventorySlot& ItemSlot);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRemoveSlot(const FItemInventorySlot& ItemSlot);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RefreshSlotInfo(const FItemInventorySlot& ItemSlot, bool IsPredicted);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateInventory();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddNewSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveSlot(const FItemInventorySlot& ItemSlot, bool IsPredicted);

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> InventoryList;

	UFUNCTION()
	void SyncInventoryWithServer();

	UFUNCTION()
	void OnItemActionRejected(TSubclassOf<AMainItemActor> ItemClass);

	TMap<FName, FDelegateHandle> PredictedActionRejectHandles;
	
	UPROPERTY()
	TMap<FName, UItemSlotWidget*> ItemSlotWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UItemSlotWidget> ItemWidgetClass;
};
