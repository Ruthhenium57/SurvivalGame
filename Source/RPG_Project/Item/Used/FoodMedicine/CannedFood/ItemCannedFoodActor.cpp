// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCannedFoodActor.h"

AItemCannedFoodActor::AItemCannedFoodActor()
{
	ItemLogic = CreateDefaultSubobject<UItemCannedFood>(TEXT("ItemCannedFoodLogic"));
}