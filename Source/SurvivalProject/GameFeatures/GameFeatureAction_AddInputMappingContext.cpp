// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_AddInputMappingContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InventoryLogMacros.h"
#include "Components/GameFrameworkComponentManager.h"
#include "SurvivalProject/Characters/SurvivalCharacterComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "SurvivalProject/System/SurvivalAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputMappingContext)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputMappingContexts();
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);

	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	{
		if (ensure(ActiveData))
		{
			Reset(*ActiveData);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	UnregisterInputMappingContexts();
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputMappingContext::IsDataValid(
	class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context),
	                                                            EDataValidationResult::Valid);

	int32 Index = 0;

	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		if (Entry.InputMappingContext.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContexts()
{
	RegisterInputMappingContextsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(
		this, &UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContextsForGameInstance);

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator();
	     WorldContextIterator; ++WorldContextIterator)
	{
		RegisterInputMappingContextsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContextsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(
			this, &UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContextsForLocalPlayer);
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(
			this, &UGameFeatureAction_AddInputMappingContext::UnregisterInputMappingContextsForLocalPlayer);

		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator();
		     LocalPlayerIterator; ++LocalPlayerIterator)
		{
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		USurvivalAssetManager& AssetManager = USurvivalAssetManager::Get();

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSetting)
					{
						continue;
					}

					// Register this IMC with the settings!
					if (UInputMappingContext* IMC = AssetManager.GetAsset(Entry.InputMappingContext))
					{
						Settings->RegisterInputMappingContext(IMC);
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputMappingContextsForGameInstanceHandle);
	RegisterInputMappingContextsForGameInstanceHandle.Reset();

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator();
	     WorldContextIterator; ++WorldContextIterator)
	{
		UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputContextMappingsForGameInstance(
	UGameInstance* GameInstance)
{
	if (GameInstance)
	{
		GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
		GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator();
		     LocalPlayerIterator; ++LocalPlayerIterator)
		{
			UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSetting)
					{
						continue;
					}

					// Register this IMC with the settings!
					if (UInputMappingContext* IMC = Entry.InputMappingContext.Get())
					{
						Settings->UnregisterInputMappingContext(IMC);
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::AddToWorld(const FWorldContext& WorldContext,
                                                           const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager =
			UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
					this, &ThisClass::HandleControlExtension, ChangeContext);
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddAbilitiesDelegate);

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APlayerController> ControllerPtr = ActiveData.ControllersAddedTo.Top();
		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::HandleControlExtension(AActor* Actor, FName EventName,
                                                                       FGameFeatureStateChangeContext ChangeContext)
{
	APlayerController* AsController = CastChecked<APlayerController>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName ==
		UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsController, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName ==
		USurvivalCharacterComponent::NAME_BindInputsNow))
	{
		AddInputMappingToPlayer(AsController->GetLocalPlayer(), ActiveData);
	}
}

void UGameFeatureAction_AddInputMappingContext::AddInputMappingToPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if (const UInputMappingContext* IMC = Entry.InputMappingContext.Get())
				{
					if (Entry.bAddInputMappingContext)
					{
						Subsystem->AddMappingContext(IMC, Entry.Priority);
					}
				}
			}
		}
		else
		{
			LOGF_FUNC(Error, "Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. "
			          "Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file.")
		}
	}
}

void UGameFeatureAction_AddInputMappingContext::RemoveInputMapping(APlayerController* PlayerController,
                                                                   FPerContextData& ActiveData)
{
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if (const UInputMappingContext* IMC = Entry.InputMappingContext.Get())
				{
					if (Entry.bAddInputMappingContext)
					{
						Subsystem->RemoveMappingContext(IMC);
					}
				}
			}
		}
	}

	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE
