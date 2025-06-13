// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "CraftingMenuWidget.generated.h"

class UCraftDataSubsystem;
class UInventoryDataSubsystem;
class UCraftDescriptionWidget;
class UCategoryButtonWidget;
class UScrollBox;
class UUniformGridPanel;
class UVerticalBox;
class UItemImageSlotWidget;
class UInventoryComponent;
class UCraftComponent;
/**
 * 
 */
UCLASS()
class CRAFTSYSTEMPLUGIN_API UCraftingMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SelectCategory(const EItemType Category);
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UCraftComponent> CraftComponent;
	
	void InitializeAll();
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UItemImageSlotWidget> ItemImageSlotWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UCategoryButtonWidget> CategoryButtonWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UCraftDescriptionWidget> CraftDescriptionWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoriesBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> CraftsGrid;

	UPROPERTY()
	TMap<int32, UItemImageSlotWidget*> GridItemsCache;

	EItemType CurrentCategory;
	TArray<EItemType> Categories;
	
	void InitializeAllItems();
	void InitializeCategories();

	UFUNCTION()
	void OnGridItemClicked(int32 ItemClass);

	void FilterGridByCategory(const EItemType Category);

	UPROPERTY()
	TObjectPtr<UInventoryDataSubsystem> InventorySubsystem;

	UPROPERTY()
	TObjectPtr<UCraftDataSubsystem> CraftSubsystem;
};
