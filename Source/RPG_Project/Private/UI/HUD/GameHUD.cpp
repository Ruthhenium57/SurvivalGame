// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/GameHUD.h"
#include "Blueprint/UserWidget.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UUserWidget>(GetWorld(), MainHUDWidgetClass);

		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}