// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThirstBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UThirstBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetThirst(float ThirstPercentage);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* ThirstBar;
};
