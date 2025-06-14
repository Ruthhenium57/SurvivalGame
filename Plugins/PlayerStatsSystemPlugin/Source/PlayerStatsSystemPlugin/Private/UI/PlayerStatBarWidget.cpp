// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerStatBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStats/PlayerStatsComponent.h"

void UPlayerStatBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (PlayerStatsComponent)
    {
        PlayerStatsComponent->OnStatValueChanged.AddDynamic(this, &UPlayerStatBarWidget::SetValue);
    }
}

void UPlayerStatBarWidget::SetValue(EPlayerStatType StatType, float NewValue, float OldValue, float MaxValue)
{
    if (StatType == PlayerStatType)
    {
        PlayerStatBar->SetPercent(NewValue / MaxValue);
        OnChangeValue(NewValue, OldValue);
    }
}