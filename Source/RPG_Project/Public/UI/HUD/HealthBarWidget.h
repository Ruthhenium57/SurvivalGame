// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetHealth(float HealthPercentage);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;
};
