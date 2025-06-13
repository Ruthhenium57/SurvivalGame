// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGameplayAbility.h"
#include "LogMacros.h"
#include "SurvivalGameplayTags.h"
#include "SurvivalAbilityCost.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/SurvivalAbilitySourceInterface.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "AbilitySystem/SurvivalGameplayEffectContext.h"
#include "Characters/SurvivalCharacterComponent.h"
#include "GameFramework/SurvivalPlayerController.h"
#include "Physics/PhysicalMaterialTagSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalGameplayAbility)


USurvivalGameplayAbility::USurvivalGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ActivationPolicy = ESurvivalAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = ESurvivalAbilityActivationGroup::Independent;

	bLogCancelation = false;
}

USurvivalAbilitySystemComponent* USurvivalGameplayAbility::GetSurvivalAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo
		        ? Cast<USurvivalAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get())
		        : nullptr);
}

ASurvivalPlayerController* USurvivalGameplayAbility::GetSurvivalPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ASurvivalPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* USurvivalGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

USurvivalCharacterComponent* USurvivalGameplayAbility::GetCharacterComponentFromActorInfo() const
{
	return (CurrentActorInfo
		        ? USurvivalCharacterComponent::FindCharacterComponent(CurrentActorInfo->AvatarActor.Get())
		        : nullptr);
}

void USurvivalGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
                                                         const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == ESurvivalAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
				(NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (
				NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool USurvivalGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayTagContainer* SourceTags,
                                                  const FGameplayTagContainer* TargetTags,
                                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	//@TODO Possibly remove after setting up tag relationships
	USurvivalAbilitySystemComponent* SurvivalASC = CastChecked<USurvivalAbilitySystemComponent>(
		ActorInfo->AbilitySystemComponent.Get());
	if (SurvivalASC->IsActivationGroupBlocked(ActivationGroup))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(SurvivalGameplayTags::Ability_ActivateFail_ActivationGroup);
		}
		return false;
	}

	return true;
}

void USurvivalGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// The ability can not block canceling if it's replaceable.
	if (!bCanBeCanceled && (ActivationGroup == ESurvivalAbilityActivationGroup::Exclusive_Replaceable))
	{
		LOGF_CATEGORY(LogSurvivalAbilitySystem, Error,
		              "SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable.",
		              *GetName())
		return;
	}

	Super::SetCanBeCanceled(bCanBeCanceled);
}

void USurvivalGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void USurvivalGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USurvivalGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USurvivalGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool USurvivalGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// Verify we can afford any additional costs
	for (const TObjectPtr<USurvivalAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void USurvivalGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	check(ActorInfo);

	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
	auto DetermineIfAbilityHitTarget = [&]()
	{
		if (ActorInfo->IsNetAuthority())
		{
			if (USurvivalAbilitySystemComponent* ASC = Cast<USurvivalAbilitySystemComponent>(
				ActorInfo->AbilitySystemComponent.Get()))
			{
				FGameplayAbilityTargetDataHandle TargetData;
				ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
				for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
				{
					if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
					{
						return true;
					}
				}
			}
		}

		return false;
	};

	// Pay any additional costs
	bool bAbilityHitTarget = false;
	bool bHasDeterminedIfAbilityHitTarget = false;
	for (const TObjectPtr<USurvivalAbilityCost>& AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				if (!bHasDeterminedIfAbilityHitTarget)
				{
					bAbilityHitTarget = DetermineIfAbilityHitTarget();
					bHasDeterminedIfAbilityHitTarget = true;
				}

				if (!bAbilityHitTarget)
				{
					continue;
				}
			}

			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

FGameplayEffectContextHandle USurvivalGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
                                                                         const FGameplayAbilityActorInfo* ActorInfo)
const
{
	FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);

	FSurvivalGameplayEffectContext* EffectContext = FSurvivalGameplayEffectContext::ExtractEffectContext(ContextHandle);
	check(EffectContext);

	check(ActorInfo);

	AActor* EffectCauser = nullptr;
	const ISurvivalAbilitySourceInterface* AbilitySource = nullptr;
	float SourceLevel = 0.0f;
	GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);

	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;

	EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
	EffectContext->AddInstigator(Instigator, EffectCauser);
	EffectContext->AddSourceObject(SourceObject);

	return ContextHandle;
}

void USurvivalGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
                                                                    FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);

	if (const FHitResult* HitResult = Spec.GetContext().GetHitResult())
	{
		if (UPhysicalMaterial* PhysMat = HitResult->PhysMaterial.Get())
		{
			UPhysicalMaterialTagSubsystem* Subsys =
			  GetWorld()->GetGameInstance()->GetSubsystem<UPhysicalMaterialTagSubsystem>();
			if (const FGameplayTagContainer* Tags = Subsys->GetTags(PhysMat))
			{
				Spec.CapturedTargetTags.GetSpecTags().AppendTags(*Tags);
			}
		}
	}
}

bool USurvivalGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
                                                                 const FGameplayTagContainer* SourceTags,
                                                                 const FGameplayTagContainer* TargetTags,
                                                                 FGameplayTagContainer* OptionalRelevantTags) const
{
	// Specialized version to handle death exclusion and AbilityTags expansion via ASC

	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(GetAssetTags()))
	{
		bBlocked = true;
	}

	const USurvivalAbilitySystemComponent* SurvivalASC = Cast<USurvivalAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (SurvivalASC)
	{
		SurvivalASC->GetAdditionalActivationTagRequirements(GetAssetTags(), AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(SurvivalGameplayTags::Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(SurvivalGameplayTags::Ability_ActivateFail_IsDead);
			}

			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

void USurvivalGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}

void USurvivalGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel,
                                                const ISurvivalAbilitySourceInterface*& OutAbilitySource,
                                                AActor*& OutEffectCauser) const
{
	OutSourceLevel = 0.0f;
	OutAbilitySource = nullptr;
	OutEffectCauser = nullptr;

	OutEffectCauser = ActorInfo->AvatarActor.Get();

	// If we were added by something that's an ability info source, use it
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	OutAbilitySource = Cast<ISurvivalAbilitySourceInterface>(SourceObject);
}
