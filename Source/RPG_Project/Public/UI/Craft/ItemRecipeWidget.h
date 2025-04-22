// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemRecipeWidget.generated.h"

struct FItemData;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UItemRecipeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NeededItemsText;

	UFUNCTION()
	void UpdateInfo(FItemData ItemData, FText ItemRecipeText);
};
