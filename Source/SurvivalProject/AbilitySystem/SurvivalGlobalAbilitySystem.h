// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "SurvivalGlobalAbilitySystem.generated.h"

class USurvivalAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<USurvivalAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayAbility>& AbilityClass, USurvivalAbilitySystemComponent* ASC);
	void RemoveFromASC(USurvivalAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<USurvivalAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(const TSubclassOf<UGameplayEffect>& EffectClass, USurvivalAbilitySystemComponent* ASC);
	void RemoveFromASC(USurvivalAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

/**
 *  USurvivalGlobalAbilitySystem
 *
 *  A system that allows you to control effects for all players
 */
UCLASS()
class SURVIVALPROJECT_API USurvivalGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	USurvivalGlobalAbilitySystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Survival")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Survival")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Survival")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Survival")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	// Register an ASC with global system and apply any active global effects/abilities
	void RegisterASC(USurvivalAbilitySystemComponent* ASC);

	// Removes an ASC from the global system, along with any active global effects/abilities
	void UnregisterASC(USurvivalAbilitySystemComponent* ASC);

private:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<TObjectPtr<USurvivalAbilitySystemComponent>> RegisteredASCs;
};
