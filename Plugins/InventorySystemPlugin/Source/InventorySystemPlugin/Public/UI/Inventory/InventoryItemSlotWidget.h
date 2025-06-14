// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Inventory/ItemData.h"
#include "InventoryItemSlotWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API UInventoryItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnUpdateItemInfo(int32 ItemID, int32 Count, FItemData ItemData);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateItemInfo(int32 ItemID, int32 Count, FItemData ItemData);
	
	void SetIsPredicted(bool bPredicted) { bIsPredicted = bPredicted; }
	bool IsPredicted() const { return bIsPredicted; }

	int32 GetQuantity() { return FCString::Atoi(*ItemQuantity->GetText().ToString()); }
	
protected:
	UPROPERTY()
	bool bIsPredicted = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemQuantity;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;
};
