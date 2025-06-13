// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalAbilitySet.h"

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "LogMacros.h"
#include "SurvivalAbilitySystemComponent.h"

void FSurvivalAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FSurvivalAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FSurvivalAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FSurvivalAbilitySet_GrantedHandles::TakeFromAbilitySystem(USurvivalAbilitySystemComponent* SurvivalASC)
{
	check(SurvivalASC)

	if (!SurvivalASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			SurvivalASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			SurvivalASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		SurvivalASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

USurvivalAbilitySet::USurvivalAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USurvivalAbilitySet::GiveToAbilitySystem(USurvivalAbilitySystemComponent* SurvivalASC,
                                              FSurvivalAbilitySet_GrantedHandles* OutGrantedHandles,
                                              UObject* SourceObject) const
{
	check(SurvivalASC);

	if (!SurvivalASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FSurvivalAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			LOGF_FUNC(Error, "GrantedGameplayAbilities[%d] on ability set [%s] is not valid.", AbilityIndex,
			          *GetNameSafe(this))
			continue;
		}

		USurvivalGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<USurvivalGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = SurvivalASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FSurvivalAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			LOGF_FUNC(Error, "GrantedGameplayEffects[%d] on ability set [%s] is not valid", EffectIndex,
			          *GetNameSafe(this))
			continue;
		}

		const UGameplayEffect* GameplayEffectCDO = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = SurvivalASC->ApplyGameplayEffectToSelf(
			GameplayEffectCDO, EffectToGrant.EffectLevel, SurvivalASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FSurvivalAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			LOGF_FUNC(Error, "GrantedAttributes[%d] on ability set [%s] is not valid", SetIndex,
					  *GetNameSafe(this))
			continue;
		}

		UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(SurvivalASC->GetOwner(), SetToGrant.AttributeSet);
		SurvivalASC->AddAttributeSetSubobject(NewAttributeSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewAttributeSet);
		}
	}
}
