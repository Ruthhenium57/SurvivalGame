// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_WorldBase.h"

void UGameFeatureAction_WorldBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);

	GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(
		this, &UGameFeatureAction_WorldBase::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
		}
	}
}

void UGameFeatureAction_WorldBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FDelegateHandle* FoundDelegate = GameInstanceStartHandles.Find(Context);
	if (ensure(FoundDelegate))
	{
		FWorldDelegates::OnStartGameInstance.Remove(*FoundDelegate);
	}
}

void UGameFeatureAction_WorldBase::HandleGameInstanceStart(UGameInstance* GameInstance,
                                                      FGameFeatureStateChangeContext ChangeContext)
{
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (ChangeContext.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, ChangeContext);
		}
	}
}
