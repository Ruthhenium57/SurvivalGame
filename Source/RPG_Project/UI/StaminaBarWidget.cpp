// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaBarWidget.h"
#include "Components/ProgressBar.h"

void UStaminaBarWidget::SetStamina(float StaminaPercentage)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(StaminaPercentage);
    }
}
