// Fill out your copyright notice in the Description page of Project Settings.


#include "HungerBarWidget.h"
#include "Components/ProgressBar.h"

void UHungerBarWidget::SetHunger(float HungerPercentage)
{
    if (HungerBar)
    {
        HungerBar->SetPercent(HungerPercentage);
    }
}

