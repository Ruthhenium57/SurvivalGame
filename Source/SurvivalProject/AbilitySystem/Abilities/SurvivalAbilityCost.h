// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Object.h"
#include "SurvivalAbilityCost.generated.h"

class USurvivalGameplayAbility;

/**
 * USurvivalAbilityCost
 *
 * Base class for costs
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class SURVIVALPROJECT_API USurvivalAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	USurvivalAbilityCost()
	{
	}

	/**
	 * Checks if we can afford this cost.
	 *
	 * A failure reason tag can be added to OptionalRelevantTags (if non-null), which can be queried
	 * elsewhere to determine how to provide user feedback
	 * 
	 * Ability and ActorInfo are guaranteed to be non-null on entry, but OptionalRelevantTags can be nullptr.
	 * 
	 * @return true if we can pay for the ability, false otherwise.
	 */
	virtual bool CheckCost(const USurvivalGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}
	/**
	 * Applies the ability's cost to the target
	 *
	 *  Notes:
	 *  - Your implementation don't need to check ShouldOnlyApplyCostOnHit(), the caller does that for you.
	 *  - Ability and ActorInfo are guaranteed to be non-null on entry.
	 */
	virtual void ApplyCost(const USurvivalGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo)
	{
	}

	/** If true, this cost should only be applied if this ability hits successfully */
	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	/** If true, this cost should only be applied if this ability hits successfully */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	bool bOnlyApplyCostOnHit = false;
};
