// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractKeyInfoWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API UInteractKeyInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractInfo(EInteractType InteractType, TArray<FKey> Keys);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> KeyIcon;
};
