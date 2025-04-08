// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "ItemRecipeWidget.h"
#include "Characters/PlayerCharacter.h"
#include "CraftDescriptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCraftDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BingWidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta = (BingWidget))
	class UTextBlock* ItemDescription;

	UPROPERTY(meta = (BingWidget))
	class UItemRecipeWidget* ItemRecipeWidget;

	UPROPERTY(meta = (BingWidget))
	class UButton* CraftButton;

	UPROPERTY(meta = (BingWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BingWidget))
	class UHorizontalBox* RecipeWidgetBox;

	UPROPERTY()
	FItemData ItemData;

	UPROPERTY()
	APlayerCharacter* PlayableCharacter;

	UPROPERTY()
	bool bCanCraftItem;

	UFUNCTION()
	void SubscribeToInventoryUpdated();

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void UpdateDescription();
};
