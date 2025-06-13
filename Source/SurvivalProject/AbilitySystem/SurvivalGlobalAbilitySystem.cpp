// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGlobalAbilitySystem.h"
#include "GameplayAbilitySpecHandle.h"
#include "SurvivalAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"


void FGlobalAppliedAbilityList::AddToASC(const TSubclassOf<UGameplayAbility>& AbilityClass,
                                         USurvivalAbilitySystemComponent* ASC)
{
	if (Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	UGameplayAbility* Ability = AbilityClass->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(Ability);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(USurvivalAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (TPair<TObjectPtr<USurvivalAbilitySystemComponent>, FGameplayAbilitySpecHandle> Pair : Handles)
	{
		if (Pair.Key != nullptr)
		{
			Pair.Key->ClearAbility(Pair.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(const TSubclassOf<UGameplayEffect>& EffectClass, USurvivalAbilitySystemComponent* ASC)
{
	if (Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}

	const UGameplayEffect* Effect = EffectClass->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(
		Effect, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(USurvivalAbilitySystemComponent* ASC)
{
	if (const FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& Pair : Handles)
	{
		if (Pair.Key != nullptr)
		{
			Pair.Key->RemoveActiveGameplayEffect(Pair.Value);
		}
	}
	Handles.Empty();
}

USurvivalGlobalAbilitySystem::USurvivalGlobalAbilitySystem()
{
}

void USurvivalGlobalAbilitySystem::ApplyAbilityToAll(const TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (AbilityClass && !AppliedAbilities.Contains(AbilityClass))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(AbilityClass);
		for (USurvivalAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(AbilityClass, ASC);
		}
	}
}

void USurvivalGlobalAbilitySystem::ApplyEffectToAll(const TSubclassOf<UGameplayEffect> EffectClass)
{
	if (EffectClass && !AppliedEffects.Contains(EffectClass))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(EffectClass);
		for (USurvivalAbilitySystemComponent* ASC : RegisteredASCs)
		{
			Entry.AddToASC(EffectClass, ASC);
		}
	}
}

void USurvivalGlobalAbilitySystem::RemoveAbilityFromAll(const TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability && AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
		Entry.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void USurvivalGlobalAbilitySystem::RemoveEffectFromAll(const TSubclassOf<UGameplayEffect> Effect)
{
	if (Effect && AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
		Entry.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void USurvivalGlobalAbilitySystem::RegisterASC(USurvivalAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Pair : AppliedAbilities)
	{
		Pair.Value.AddToASC(Pair.Key, ASC);
	}
	for (auto& Pair : AppliedEffects)
	{
		Pair.Value.AddToASC(Pair.Key, ASC);
	}

	RegisteredASCs.AddUnique(ASC);
}

void USurvivalGlobalAbilitySystem::UnregisterASC(USurvivalAbilitySystemComponent* ASC)
{
	check(ASC);

	for (auto& Pair : AppliedAbilities)
	{
		Pair.Value.RemoveFromASC(ASC);
	}
	for (auto& Pair : AppliedEffects)
	{
		Pair.Value.RemoveFromASC(ASC);
	}

	RegisteredASCs.Remove(ASC);
}
