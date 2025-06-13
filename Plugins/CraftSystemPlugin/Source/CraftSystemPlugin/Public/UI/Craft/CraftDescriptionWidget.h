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
class CRAFTSYSTEMPLUGIN_API UCraftDescriptionWidget : public UUserWidget
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
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
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
