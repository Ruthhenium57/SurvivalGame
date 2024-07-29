// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemUsedActor.h"
#include "ItemFoodMedicine.h"
#include "ItemFoodMedicineActor.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AItemFoodMedicineActor : public AItemUsedActor
{
	GENERATED_BODY()
	
public:
	AItemFoodMedicineActor();

	void CallUse();
};