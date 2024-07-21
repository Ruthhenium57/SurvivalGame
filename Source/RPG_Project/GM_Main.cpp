// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Main.h"
#include "GameHUD.h"

AGM_Main::AGM_Main() : Super()
{
	ConstructorHelpers::FClassFinder<APawn> MainSurvivalCharacter(TEXT("/Game/BP/Character/BP_MainCharacter"));

	DefaultPawnClass = MainSurvivalCharacter.Class;
	HUDClass = AGameHUD::StaticClass();
};