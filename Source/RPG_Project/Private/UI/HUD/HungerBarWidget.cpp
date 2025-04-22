// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/HungerBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStats/PlayerStatsComponent.h"

void UHungerBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (PlayerStatsComponent)
    {
        PlayerStatsComponent->OnHungerChanged.AddUObject(this, &UHungerBarWidget::SetHunger);
    }
}

void UHungerBarWidget::SetHunger(float HungerPercentage)
{
    if (HungerBar)
    {
        HungerBar->SetPercent(HungerPercentage);
        OnSetHunger(HungerPercentage);
    }
}