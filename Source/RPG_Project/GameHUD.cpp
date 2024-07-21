// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "MainHUDWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AGameHUD::AGameHUD()
{
	static ConstructorHelpers::FClassFinder<UMainHUDWidget> WidgetClass(TEXT("/Game/UI/MainHUDWidget"));
	if (WidgetClass.Succeeded())
	{
		MainHUDWidgetClass = WidgetClass.Class;
	}
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UMainHUDWidget>(GetWorld(), MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}