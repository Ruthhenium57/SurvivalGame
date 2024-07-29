// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemFoodMedicineActor.h"

AItemFoodMedicineActor::AItemFoodMedicineActor()
{
	
}

void AItemFoodMedicineActor::CallUse()
{
	if (UItemFoodMedicine* ItemFoodMedicine = Cast<UItemFoodMedicine>(ItemLogic))
	{
		ItemFoodMedicine->Use();
	}
}