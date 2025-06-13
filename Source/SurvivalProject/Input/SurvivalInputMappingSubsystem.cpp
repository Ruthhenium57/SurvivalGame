// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalInputMappingSubsystem.h"
#include "LogMacros.h"
#include "EnhancedInputSubsystems.h"
#include "IMCDataAsset.h"
#include "InputMappingContext.h"
#include "System/SurvivalAssetManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalInputMappingSubsystem)

void USurvivalInputMappingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LOGF_FUNC(Warning, "Start input init")
	
	USurvivalAssetManager& AssetManager = USurvivalAssetManager::Get();
	const UIMCDataAsset& DataAsset = AssetManager.GetIMCDataAsset();
	if (!DataAsset.IsAsset()) return;

	LOGF_FUNC(Warning, "HasDataAsset")
	
	UGameInstance* GI = GetGameInstance();
	LOGF_FUNC(Warning, "LocalPlayers count: %d", GI->GetLocalPlayers().Num())
	const TArray<ULocalPlayer*>& LocalPlayers = GI->GetLocalPlayers();
	LOGF_FUNC(Warning, "LocalPlayersLength: %d", LocalPlayers.Num())
	for (ULocalPlayer* LocalPlayer : LocalPlayers)
	{
		LOGF_FUNC(Warning, "StartLoop, LP is: %s", *LocalPlayer->GetName())
		if (!LocalPlayer) continue;
		LOGF_FUNC(Warning, "HasLP")

		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (!Subsystem) return;
		LOGF_FUNC(Warning, "HasSubsystem")

		UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings();
		if (!Settings) return;
		LOGF_FUNC(Warning, "HasSetting")
		
		for (auto& SoftDA : DataAsset.AllIMCSettings)
		{
			UIMCAndPriority* IMCAndPriority = AssetManager.GetAsset<UIMCAndPriority>(SoftDA);
			if (!IMCAndPriority) return;
			LOGF_FUNC(Warning, "HasIMCAndP")

			UInputMappingContext* IMC = IMCAndPriority->InputMappingContext.Get();
			if (!IMC) return;
			LOGF_FUNC(Warning, "HasIMC")
			
			FName ID = IMCAndPriority->ContextId.IsNone() ? IMC->GetFName() : IMCAndPriority->ContextId;
			
			// Skip entries that don't want to be registered
			if (!IMCAndPriority->bRegisterWithSetting)
			{
				continue;
			}

			// Register this IMC with the settings!
			Settings->RegisterInputMappingContext(IMC);
			ContextRegistry.Add(ID, IMCAndPriority);

			if (IMCAndPriority->bIsGlobal && IMC)
			{
				Subsystem->AddMappingContext(IMC, IMCAndPriority->Priority);
				GlobalContexts.Add(ID);
				LOGF_FUNC(Warning, "AllGood")
			}
		}
	}
}

void USurvivalInputMappingSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UIMCAndPriority* USurvivalInputMappingSubsystem::GetContextByName(FName Name) const
{
	if (ContextRegistry.IsEmpty())
	{
		LOGF_FUNC(Error, "No ContextRegistry")
		return nullptr;
	}
	
	UIMCAndPriority* IMCAndP = *ContextRegistry.Find(Name);
	return IMCAndP ? IMCAndP : nullptr;
}
