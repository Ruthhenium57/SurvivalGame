// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPawnExtensionComponent.h"

#include "LogMacros.h"
#include "SurvivalGameplayTags.h"
#include "SurvivalPawnData.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/SurvivalGameInstance.h"
#include "Input/SurvivalInputMappingSubsystem.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalPawnExtensionComponent)

const FName USurvivalPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

USurvivalPawnExtensionComponent::USurvivalPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

// void USurvivalPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME(USurvivalPawnExtensionComponent, PawnData);
// }

void USurvivalPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr),
	                 TEXT("SurvivalPawnExtensionComponent on [%s] can only be added to Pawn actors."),
	                 *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1),
	                 TEXT("Only one SurvivalPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void USurvivalPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(SurvivalGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void USurvivalPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void USurvivalPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void USurvivalPawnExtensionComponent::SetPawnData(const USurvivalPawnData* InPawnData)
{
	if (!InPawnData) return;

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

bool USurvivalPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
                                                         const FGameplayTag CurrentState,
                                                         const FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == SurvivalGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}

	if (CurrentState == SurvivalGameplayTags::InitState_Spawned && DesiredState ==
		SurvivalGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	if (CurrentState == SurvivalGameplayTags::InitState_DataAvailable && DesiredState ==
		SurvivalGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, SurvivalGameplayTags::InitState_DataAvailable);
	}

	if (CurrentState == SurvivalGameplayTags::InitState_DataInitialized && DesiredState ==
		SurvivalGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void USurvivalPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
                                                            const FGameplayTag CurrentState,
                                                            const FGameplayTag DesiredState)
{
	if (DesiredState == SurvivalGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void USurvivalPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == SurvivalGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void USurvivalPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {
		SurvivalGameplayTags::InitState_Spawned, SurvivalGameplayTags::InitState_DataAvailable,
		SurvivalGameplayTags::InitState_DataInitialized, SurvivalGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void USurvivalPawnExtensionComponent::InitializeAbilitySystem(USurvivalAbilitySystemComponent* InASC,
                                                              AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	LOGF_FUNC(Verbose, "Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] ", *GetNameSafe(InASC),
	          *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if (ExistingAvatar && (ExistingAvatar != Pawn))
	{
		LOGF_FUNC(Log, "Existing avatar (authority=%d)", ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (USurvivalPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	OnAbilitySystemInitialized.Broadcast();
}

void USurvivalPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(SurvivalGameplayTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor())
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void USurvivalPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void USurvivalPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void USurvivalPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void USurvivalPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void USurvivalPawnExtensionComponent::OnAbilitySystemUninitialized_Register(
	const FSimpleMulticastDelegate::FDelegate& Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}