// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(meta = (BindWidget))
    class UHealthBarWidget* HealthBarWidget;

    UPROPERTY(meta = (BindWidget))
    class UStaminaBarWidget* StaminaBarWidget;

    UPROPERTY(meta = (BindWidget))
    class UThirstBarWidget* ThirstBarWidget;

    UPROPERTY(meta = (BindWidget))
    class UHungerBarWidget* HungerBarWidget;

    UPROPERTY(meta = (BindWidget))
    class UInteractionInfoWidget* InteractionInfoWidget;

    UPROPERTY(meta = (BindWidget))
    class UInvenroryWidget* InventoryWidget;
};
