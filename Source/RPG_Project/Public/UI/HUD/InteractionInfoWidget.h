// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInteractionInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInteractInfo(FString NameTextBlock);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideInteractInfo();

protected:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* InteractionInfoBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Take;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Put;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Open;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Close;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Interact;
};
