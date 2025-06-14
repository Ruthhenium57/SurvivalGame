// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatBarWidget.generated.h"

enum class EPlayerStatType : uint8;
class UPlayerStatsComponent;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PLAYERSTATSSYSTEMPLUGIN_API UPlayerStatBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnChangeValue(float NewValue, float OldValue);
	
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetValue(EPlayerStatType StatType, float NewValue, float OldValue, float MaxValue);

	UPROPERTY()
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	EPlayerStatType PlayerStatType;
	
protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    TObjectPtr<UProgressBar> PlayerStatBar;
};
