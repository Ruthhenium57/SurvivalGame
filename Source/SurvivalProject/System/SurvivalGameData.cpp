// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGameData.h"
#include "SurvivalAssetManager.h"

USurvivalGameData::USurvivalGameData()
{
}

const USurvivalGameData& USurvivalGameData::Get()
{
	return USurvivalAssetManager::Get().GetGameData();
}
