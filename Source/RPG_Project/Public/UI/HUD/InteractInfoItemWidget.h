// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractInfoItemWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInteractInfoItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractInfo(EInteractType InteractType, FKey Key);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> KeyIcon;
};
