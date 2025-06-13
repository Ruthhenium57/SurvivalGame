// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonGameInstance.h"
#include "GameplayTagContainer.h"
#include "SurvivalProject/LogMacros.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonGameInstance)

UCommonGameInstance::UCommonGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

int32 UCommonGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, UserId);
	if (ReturnVal != INDEX_NONE)
	{
		if (!PrimaryPlayer.IsValid())
		{
			//LOGF_FUNC(Log, "AddLocalPlayer: Set %s to Primary Player", *NewPlayer->GetName())
			PrimaryPlayer = NewPlayer;
		}
		
		//@TODO: GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(NewPlayer));
	}
	
	return ReturnVal;
}

bool UCommonGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		//TODO: do we want to fall back to another player?
		PrimaryPlayer.Reset();
		//LOGF_FUNC(Log, "RemoveLocalPlayer: Unsetting Primary Player from %s", *ExistingPlayer->GetName())
	}
	//@TODO: GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerDestroyed(Cast<UCommonLocalPlayer>(ExistingPlayer));

	return Super::RemoveLocalPlayer(ExistingPlayer);
}

void UCommonGameInstance::Init()
{
	Super::Init();
}
