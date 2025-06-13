// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SurvivalPawnData.generated.h"

struct FInputMappingContextAndPriority;
class USurvivalInputConfig;
class USurvivalAbilitySet;

/**
 *	USurvivalPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Survival Pawn Data"))
class SURVIVALPROJECT_API USurvivalPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USurvivalPawnData(const FObjectInitializer& ObjectInitializer);
	
	// Ability sets to grant to this pawn's ability system
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|Ability")
	TArray<TObjectPtr<USurvivalAbilitySet>> AbilitySets;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|Input")
	TObjectPtr<USurvivalInputConfig> InputConfig;
};
