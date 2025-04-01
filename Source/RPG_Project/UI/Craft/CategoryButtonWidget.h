// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPG_Project/ItemData.h"
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
	class UTextBlock* CategoryNameBlock;

	UPROPERTY(meta = (BindWidget))
	class UTexture2D* CategoryIcon;

	UPROPERTY(meta = (BindWidget))
	class UButton* CategoryButton;

	UPROPERTY()
	EItemType ItemCategory;

	UFUNCTION()
	void UpdateWidgetData();

	UFUNCTION()
	void SetIcon(const FString& IconPath);

	UPROPERTY(BlueprintAssignable)
	FOnCategorySelectedDelegate OnCategorySelected;
};
