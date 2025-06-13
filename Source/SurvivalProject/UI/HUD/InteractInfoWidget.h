// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/InteractableInterface.h"
#include "InteractInfoWidget.generated.h"

struct FActionKeyConnector;
class UInputAction;
class UInteractKeyInfoWidget;
class UTextBlock;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API UInteractInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateInteractContainer(TMap<EInteractType, TArray<FKey>> Bindings,
	                             FName ObjectName);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> InteractionInfoBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UInteractKeyInfoWidget> InteractInfoItemWidgetClass;
};
