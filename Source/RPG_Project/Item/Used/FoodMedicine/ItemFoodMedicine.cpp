// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemFoodMedicine.h"

void UItemFoodMedicine::Use()
{
	Super::Use();
	UE_LOG(LogTemp, Display, TEXT("FoodMedicine used"));
}
