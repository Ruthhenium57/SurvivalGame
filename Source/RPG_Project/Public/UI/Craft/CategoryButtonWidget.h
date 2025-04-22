// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CategoryButtonWidget.generated.h"

class UButton;
class UTextBlock;
enum class EItemType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelectedDelegate, const EItemType, ItemType);

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
    void UpdateWidgetData();

    UPROPERTY(BlueprintAssignable)
    FOnCategorySelectedDelegate OnCategorySelected;

	UPROPERTY()
	EItemType ItemCategory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name;
	
protected:
	UFUNCTION()
	void OnButtonClicked();

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
};
