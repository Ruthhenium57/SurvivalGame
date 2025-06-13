// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CommonLocalPlayer.h"
#include "SurvivalLocalPlayer.generated.h"

class UInputMappingContext;
struct FSwapAudioOutputResult;
/**
 * USurvivalLocalPlayer
 *
 * Base LP for this project
 */
UCLASS()
class SURVIVALPROJECT_API USurvivalLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	// /** Gets the local settings for this player, this is read from config files at process startup and is always valid */
	// UFUNCTION()
	// USurvivalSettingsLocal* GetLocalSettings() const;
	//
	// /** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	// UFUNCTION()
	// USurvivalSettingsShared* GetSharedSettings() const;
	//
	// /** Starts an async request to load the shared settings, this will call OnSharedSettingsLoaded after loading or creating new ones */
	// void LoadSharedSettingsFromDisk(bool bForceLoad = false);

protected:
	//void OnSharedSettingsLoaded(USurvivalSettingsShared* LoadedOrCreatedSettings);
	//
	// void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	//
	// UFUNCTION()
	// void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

private:
	// UPROPERTY(Transient)
	// mutable TObjectPtr<USurvivalSettingsShared> SharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
