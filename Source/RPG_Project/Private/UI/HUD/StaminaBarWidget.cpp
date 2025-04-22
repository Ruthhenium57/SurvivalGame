// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/StaminaBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStats/PlayerStatsComponent.h"

void UStaminaBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (PlayerStatsComponent)
    {
        PlayerStatsComponent->OnStaminaChanged.AddUObject(this, &UStaminaBarWidget::SetStamina);
    }
}

void UStaminaBarWidget::SetStamina(float StaminaPercentage)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(StaminaPercentage);
        OnSetStamina(StaminaPercentage);
    }
}