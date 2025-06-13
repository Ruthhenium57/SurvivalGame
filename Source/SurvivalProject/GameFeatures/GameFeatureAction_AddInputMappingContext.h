// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldBase.h"
#include "UObject/SoftObjectPtr.h"
#include "GameFeatureAction_AddInputMappingContext.generated.h"

struct FComponentRequestHandle;
class UInputMappingContext;

USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client, Server"))
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool bAddInputMappingContext = false;

	// If true, then this mapping context will be registered with the settings when this game feature action is registered.
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSetting;
};
/**
 * 
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
class UGameFeatureAction_AddInputMappingContext final : public UGameFeatureAction_WorldBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~ End of UGameFeatureAction interface

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FInputMappingContextAndPriority> InputMappings;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	// Delegate for when the game instance is changed to register IMC's
	FDelegateHandle RegisterInputMappingContextsForGameInstanceHandle;

	// Registers owned Input Mapping Contexts to the Input Registry Subsystem. Also binds onto the start of GameInstances and the adding/removal of Local Players.
	void RegisterInputMappingContexts();
	
	// Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified GameInstance. This also gets called by a GameInstance Start.
	void RegisterInputMappingContextsForGameInstance(UGameInstance* GameInstance);

	// Registers owned Input Mapping Contexts to the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is added.
	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	// Unregisters owned Input Mapping Contexts from the Input Registry Subsystem. Also unbinds from the start of GameInstances and the adding/removal of Local Players.
	void UnregisterInputMappingContexts();

	// Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified GameInstance.
	void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

	// Unregisters owned Input Mapping Contexts from the Input Registry Subsystem for a specified Local Player. This also gets called when a Local Player is removed.
	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

	//~ Begin UGameFeatureAction_WorldBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End of UGameFeatureAction_WorldBase interface

	void Reset(FPerContextData& ActiveData);
	void HandleControlExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputMappingToPlayer(UPlayer* Player, FPerContextData& ActiveData);
	void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
};
