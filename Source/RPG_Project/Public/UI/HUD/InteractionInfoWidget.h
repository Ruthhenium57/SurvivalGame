// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractionInfoWidget.generated.h"

class UInputAction;
class UInteractInfoItemWidget;
class UTextBlock;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInteractionInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractContainer(const TArray<EInteractType>& Interacts, const TMap<EInteractType, FKey>& Bindings, FName ObjectName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> InteractionInfoBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInteractInfoItemWidget> InteractInfoItemWidgetClass;
};
