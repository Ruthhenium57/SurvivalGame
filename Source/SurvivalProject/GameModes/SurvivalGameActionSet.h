// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivalGameActionSet.generated.h"

class UGameFeatureAction;
/**
 * 
 */
UCLASS(BlueprintType, NotBlueprintable)
class SURVIVALPROJECT_API USurvivalGameActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USurvivalGameActionSet();

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditAnywhere, Instanced, Category="Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
