// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SurvivalGameplayAbility.generated.h"

class ISurvivalAbilitySourceInterface;
class USurvivalAbilityCost;
class ASurvivalPlayerController;
class USurvivalCharacterComponent;
class ASurvivalCharacter;

/**
 *	ESurvivalAbilityActivationPolicy
 *
 *	How should the ability be activated
 */
UENUM(BlueprintType)
enum class ESurvivalAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered
	OnInputTriggered,

	// Continually try to activate the ability while the input is active
	WhileInputActive,

	// When pawn is assigned
	OnSpawn,

	MAX UMETA(Hidden)
};

/**
 *	ESurvivalAbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities
 */
UENUM(BlueprintType)
enum class ESurvivalAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating
	Exclusive_Blocking,

	MAX UMETA(Hidden)
};

/**
 *	FSurvivalAbilityMontageFailureMessage
 *
 *	Failure reason that can be used to play an animation montage when a failure occurs
 */
USTRUCT(BlueprintType)
struct FSurvivalAbilityMontageFailureMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

/**
 *	USurvivalGameplayAbility
 *
 *	The base gameplay ability class in SurvivalProject
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class in SurvivalProject"))
class SURVIVALPROJECT_API USurvivalGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class USurvivalAbilitySystemComponent;

public:
	USurvivalGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Survival|Ability")
	USurvivalAbilitySystemComponent* GetSurvivalAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|Ability")
	ASurvivalPlayerController* GetSurvivalPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|Ability")
	USurvivalCharacterComponent* GetCharacterComponentFromActorInfo() const;

	ESurvivalAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	ESurvivalAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:
	//~ Begin of UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	                                                       const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	                                                  FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	                                               const FGameplayTagContainer* SourceTags = nullptr,
	                                               const FGameplayTagContainer* TargetTags = nullptr,
	                                               OUT FGameplayTagContainer* OptionalRelevantTags = nullptr)
	const override;
	//~ End of UGameplayAbility interface

	virtual void OnPawnAvatarSet();

	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              float& OutSourceLevel, const ISurvivalAbilitySourceInterface*& OutAbilitySource,
	                              AActor*& OutEffectCauser) const;

	// Called when this ability is granted to the ability system component.
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	// Called when this ability is removed from the ability system component.
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	// Called when the ability system is initialized with a pawn avatar.
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|Ability Activation")
	ESurvivalAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|Ability Activation")
	ESurvivalAbilityActivationGroup ActivationGroup;

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<USurvivalAbilityCost>> AdditionalCosts;

	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
	bool bLogCancelation;
};
