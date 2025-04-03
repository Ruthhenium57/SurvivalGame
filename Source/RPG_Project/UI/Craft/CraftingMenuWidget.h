// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CategoryButtonWidget.h"
#include "ItemImageSlotWidget.h"
#include "RPG_Project/PlayableCharacter.h"
#include "CraftingMenuWidget.generated.h"

class UItemImageSlotWidget;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCraftingMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* CategoriesBox;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* CraftsGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCategoryButtonWidget> CategoryButton;

	UPROPERTY()
	APlayableCharacter* OwningPlayer;

	UPROPERTY()
	TMap<TSubclassOf<AMainItemActor>, UItemImageSlotWidget*> GridItemsCache;

	UPROPERTY()
	TArray<EItemType> Categories;

	UPROPERTY()
	EItemType CurrentCategory;

	UPROPERTY()
	FTimerHandle InitializeRestartTimerHandle;

	UFUNCTION()
	void InitializeAll();

	UFUNCTION()
	void InitializeAllItems();

	UFUNCTION()
	void InitializeCategories();

	UFUNCTION()
	void OnGridItemClicked(const TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION()
	void FilterGridByCategory(const EItemType Category);

	UFUNCTION()
	void SelectCategory(const EItemType Category);
};
