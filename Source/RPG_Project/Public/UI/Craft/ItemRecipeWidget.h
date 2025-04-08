// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "ItemRecipeWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UItemRecipeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NeededItemsText;

	UFUNCTION()
	void UpdateInfo(FItemData ItemData, FText ItemRecipeText);
};
