// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalPlayerState.h"

#include "AbilitySystemComponent.h"
#include "LogMacros.h"
#include "SurvivalPlayerController.h"
#include "AbilitySystem/SurvivalAbilitySet.h"
#include "AbilitySystem/SurvivalAbilitySystemComponent.h"
#include "Characters/SurvivalPawnData.h"
#include "Characters/SurvivalPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalPlayerState)

const FName ASurvivalPlayerState::NAME_SurvivalAbilityReady(TEXT("SurvivalAbilityReady"));

ASurvivalPlayerState::ASurvivalPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<USurvivalAbilitySystemComponent>(
		this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

ASurvivalPlayerController* ASurvivalPlayerState::GetSurvivalPlayerController() const
{
	return Cast<ASurvivalPlayerController>(GetOwner());
}

UAbilitySystemComponent* ASurvivalPlayerState::GetAbilitySystemComponent() const
{
	return GetSurvivalAbilitySystemComponent();
}

void ASurvivalPlayerState::SetPawnData(const USurvivalPawnData* InPawnData)
{
	if(!InPawnData)
	{
		LOGF_FUNC(Error, "Trying to set PawnData but InPawnData is nullptr.")
		return;
	};

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		LOGF_FUNC(Error, "Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s].",
		          *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData))
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const USurvivalAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_SurvivalAbilityReady);

	ForceNetUpdate();
}

void ASurvivalPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	
	SetPawnData(DefaultPawnData);
}

void ASurvivalPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (USurvivalPawnExtensionComponent* PawnExtComp =
		USurvivalPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void ASurvivalPlayerState::OnRep_PawnData()
{
}

void ASurvivalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}
