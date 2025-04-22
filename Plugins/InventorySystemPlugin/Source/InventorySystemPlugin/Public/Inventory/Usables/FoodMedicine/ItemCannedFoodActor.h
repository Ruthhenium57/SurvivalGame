// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemFoodMedicineActor.h"
#include "ItemCannedFoodActor.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API AItemCannedFoodActor : public AItemFoodMedicineActor
{
	GENERATED_BODY()
	
public:
	AItemCannedFoodActor();

	void Use();
};
