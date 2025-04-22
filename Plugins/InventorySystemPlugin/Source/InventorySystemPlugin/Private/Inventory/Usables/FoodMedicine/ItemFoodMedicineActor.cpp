// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Usables/FoodMedicine/ItemFoodMedicineActor.h"

AItemFoodMedicineActor::AItemFoodMedicineActor()
{
	ChangeHealth = 0;
	ChangeHunger = 0;
	ChangeThirst = 0;
}

void AItemFoodMedicineActor::Use()
{
	Super::Use();
	ChangeStats();
	Destroy();
}

void AItemFoodMedicineActor::ChangeStats()
{
	
}
