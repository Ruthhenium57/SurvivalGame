// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MainItemActor.h"
#include "ItemUsedActor.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMPLUGIN_API AItemUsedActor : public AMainItemActor
{
	GENERATED_BODY()
	
public:
	AItemUsedActor();
	
	virtual void Tick(float DeltaTime) override;

	virtual void Use();

protected:
	virtual void BeginPlay() override;
};
