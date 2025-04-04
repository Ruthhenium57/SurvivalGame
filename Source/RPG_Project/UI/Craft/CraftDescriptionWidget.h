// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemRecipeWidget.h"
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

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void UpdateDescription(const FString& Name, const FString& Description);
};
