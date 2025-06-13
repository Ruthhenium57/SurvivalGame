// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalAbilityTagRelationshipMapping.h"


void USurvivalAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (const FGameplayTag& AbilityTag : AbilityTags)
	{
		const FSurvivalAbilityTagRelationship* Relationship = MapAbilityTagRelationships.Find(AbilityTag);
		if (Relationship)
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Relationship->AbilityTagsToBlock);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Relationship->AbilityTagsToCancel);
			}
		}
	}
}

void USurvivalAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(
	const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutRequiredTags,
	FGameplayTagContainer* OutBlockedTags) const
{
	for (const FGameplayTag& AbilityTag : AbilityTags)
	{
		const FSurvivalAbilityTagRelationship* Relationship = MapAbilityTagRelationships.Find(AbilityTag);
		if (Relationship)
		{
			if (OutRequiredTags)
			{
				OutRequiredTags->AppendTags(Relationship->ActivationRequiredTags);
			}
			if (OutBlockedTags)
			{
				OutBlockedTags->AppendTags(Relationship->ActivationBlockedTags);
			}
		}
	}
}

bool USurvivalAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& ActionTag) const
{
	const FSurvivalAbilityTagRelationship* Tags = MapAbilityTagRelationships.Find(ActionTag);
	if (Tags && Tags->AbilityTagsToCancel.HasAny(AbilityTags))
	{
		return true;
	}
	return false;
}
