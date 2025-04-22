// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/ThirstBarWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerStats/PlayerStatsComponent.h"

void UThirstBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
	
    if (PlayerStatsComponent)
    {
        PlayerStatsComponent->OnThirstChanged.AddUObject(this, &UThirstBarWidget::SetThirst);
    }
}

void UThirstBarWidget::SetThirst(float ThirstPercentage)
{
    if (ThirstBar)
    {
        ThirstBar->SetPercent(ThirstPercentage);
        OnSetThirst(ThirstPercentage);
    }
}