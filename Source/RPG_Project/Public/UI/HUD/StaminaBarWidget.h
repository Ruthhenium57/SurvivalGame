// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

class UPlayerStatsComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSetStamina(float StaminaPercentage);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetStamina(float StaminaPercentage);
	
	UPROPERTY()
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;
};
