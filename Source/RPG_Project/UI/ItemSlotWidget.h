// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateItemInfo(FItemInventorySlot ItemSlot);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemQuantity;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;
};
