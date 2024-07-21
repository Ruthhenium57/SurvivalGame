// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AGameHUD();

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMainHUDWidget> MainHUDWidgetClass;

public:

	UPROPERTY()
	UMainHUDWidget* MainHUDWidget;
};
