// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/ItemData.h"
#include "CategoryButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelectedDelegate, const EItemType, ItemType);

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

	UPROPERTY()
	EItemType ItemCategory;

	UFUNCTION()
	void UpdateWidgetData();

	UPROPERTY(BlueprintAssignable)
	FOnCategorySelectedDelegate OnCategorySelected;
};
