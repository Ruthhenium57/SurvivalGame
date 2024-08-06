// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCannedFoodActor.h"

AItemCannedFoodActor::AItemCannedFoodActor()
{
	ItemName = "Canned Food";
	MaxStack = 3;
	InteractTextBlockName = "Take";
	ChangeHunger = 15;
	ChangeThirst = 5;
	ChangeHealth = 5;
}

void AItemCannedFoodActor::Use()
{
	Super::Use();
}
