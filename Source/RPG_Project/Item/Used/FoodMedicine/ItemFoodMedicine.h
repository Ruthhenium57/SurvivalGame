// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ItemUsed.h"
#include "ItemFoodMedicine.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RPG_PROJECT_API UItemFoodMedicine : public UItemUsed
{
	GENERATED_BODY()
	
public:
	virtual void Use() override;
};