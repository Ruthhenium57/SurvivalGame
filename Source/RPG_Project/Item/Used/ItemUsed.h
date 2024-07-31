// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainItem.h"
#include "ItemUsed.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class RPG_PROJECT_API UItemUsed : public UMainItem
{
	GENERATED_BODY()
	
public:
	virtual void Use();

	virtual bool Interact(ACharacter* Character) override;
};
