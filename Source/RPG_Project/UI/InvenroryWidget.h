// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Item/InventoryComponent.h"
#include "ItemSlotWidget.h"
#include "InvenroryWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInvenroryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateSlotInfo(FItemInventorySlot ItemSlot);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateInventory(TArray<FItemInventorySlot> ItemSlots);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddNewSlot(FItemInventorySlot ItemSlot);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveSlot(FItemInventorySlot ItemSlot);

protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* InventoryList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UItemSlotWidget> ItemWidgetClass;
};
