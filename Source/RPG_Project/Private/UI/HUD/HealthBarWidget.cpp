// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStats/PlayerStatsComponent.h"

void UHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (PlayerStatsComponent)
    {
        PlayerStatsComponent->OnHealthChanged.AddUObject(this, &UHealthBarWidget::SetHealth);
    }
}

void UHealthBarWidget::SetHealth(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
        OnSetHealth(HealthPercentage);
    }
}