// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "Inventory/InventoryComponent.h"
#include "Craft/CraftComponent.h"
#include "CraftingMenuWidget.generated.h"

class UCraftDescriptionWidget;
class UCategoryButtonWidget;
class UScrollBox;
class UUniformGridPanel;
class UVerticalBox;
class AMainItemActor;
class UItemImageSlotWidget;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCraftingMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SelectCategory(const EItemType Category);
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UCraftComponent> CraftComponent;
	
	UFUNCTION()
	void InitializeAll();
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemImageSlotWidget> ItemImageSlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCategoryButtonWidget> CategoryButtonWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCraftDescriptionWidget> CraftDescriptionWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> CategoriesBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> CraftsGrid;

	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, UItemImageSlotWidget*> GridItemsCache;

	UPROPERTY()
	EItemType CurrentCategory;
	
	UPROPERTY()
	TArray<EItemType> Categories;

	UPROPERTY()
	FTimerHandle InitializeRestartTimerHandle;
	
	UFUNCTION()
	void InitializeAllItems();

	UFUNCTION()
	void InitializeCategories();

	UFUNCTION()
	void OnGridItemClicked(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION()
	void FilterGridByCategory(const EItemType Category);

	UPROPERTY()
	TObjectPtr<UInventoryDataSubsystem> InventorySubsystem;
};
