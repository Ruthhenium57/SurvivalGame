// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HungerBarWidget.generated.h"

class UPlayerStatsComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UHungerBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSetHunger(float HungerPercentage);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHunger(float HungerPercentage);
	
	UPROPERTY()
    TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HungerBar;
};
