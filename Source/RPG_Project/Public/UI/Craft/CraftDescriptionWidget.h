// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "CraftDescriptionWidget.generated.h"

struct FItemInventorySlot;
class UInventoryComponent;
class UCraftComponent;
class UHorizontalBox;
class UImage;
class UButton;
class UItemRecipeWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCraftDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UpdateDescription();

	UPROPERTY()
	FItemData ItemData;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemRecipeWidget> ItemRecipeWidgetClass;
	
	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UTextBlock> ItemDescription;

	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UItemRecipeWidget> ItemRecipeWidget;

	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UButton> CraftButton;

	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(meta = (BingWidget))
	TObjectPtr<UHorizontalBox> RecipeWidgetBox;
	
	bool bCanCraftItem;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UCraftComponent> CraftComponent;

	UFUNCTION()
	void SubscribeToInventoryUpdated();

	UFUNCTION()
	void OnButtonClicked();
};
