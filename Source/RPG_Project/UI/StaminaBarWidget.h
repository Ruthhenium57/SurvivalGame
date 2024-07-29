// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetStamina(float StaminaPercentage);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaBar;
};
