// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThirstBarWidget.generated.h"

class UPlayerStatsComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UThirstBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnSetThirst(float ThirstPercentage);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetThirst(float ThirstPercentage);
	
	UPROPERTY()
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ThirstBar;
};
