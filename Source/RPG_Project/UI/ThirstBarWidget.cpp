// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirstBarWidget.h"
#include "Components/ProgressBar.h"

void UThirstBarWidget::SetThirst(float ThirstPercentage)
{
    if (ThirstBar)
    {
        ThirstBar->SetPercent(ThirstPercentage);
    }
}

