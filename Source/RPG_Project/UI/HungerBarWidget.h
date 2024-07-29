// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HungerBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UHungerBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetHunger(float HungerPercentage);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HungerBar;
};
