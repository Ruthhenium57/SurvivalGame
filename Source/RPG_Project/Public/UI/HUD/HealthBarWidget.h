// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UPlayerStatsComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSetHealth(float HealthPercentage);
	
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetHealth(float HealthPercentage);

	UPROPERTY()
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;
	
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;
};
