// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainItemActor.h"
#include "ItemUsedActor.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AItemUsedActor : public AMainItemActor
{
	GENERATED_BODY()
	
public:
	AItemUsedActor();
	
	virtual void Tick(float DeltaTime) override;

	virtual void Use();

protected:
	virtual void BeginPlay() override;
};
