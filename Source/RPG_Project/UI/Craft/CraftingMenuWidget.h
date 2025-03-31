// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CategoryButtonWidget.h"
#include "CraftingMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UCraftingMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* CategoriesBox;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* CraftsGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UCategoryButtonWidget> CategoryButton;
};
