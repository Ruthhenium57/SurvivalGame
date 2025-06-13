// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "SurvivalCharacterComponent.generated.h"

class UIMCAndPriority;
struct FInputMappingContextAndPriority;
struct FInputActionValue;
class USurvivalInputConfig;

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class SURVIVALPROJECT_API USurvivalCharacterComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USurvivalCharacterComponent(const FObjectInitializer& ObjectInitializer);

	// Return the CharacterComponent if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Survival|Character")
	static USurvivalCharacterComponent* FindCharacterComponent(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<USurvivalCharacterComponent>() : nullptr;
	}

	TArray<UIMCAndPriority*> GetInputMappings();
	
	// Add input config
	void AddAdditionalInputConfig(const USurvivalInputConfig* InputConfig);

	// Remove input config if it has been added
	void RemoveAdditionalInputConfig(const USurvivalInputConfig* InputConfig);

	// True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added
	bool IsReadyToBindInputs() const;

	// The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind
	static const FName NAME_BindInputsNow;

	//The name of this component-implemented feature
	static const FName NAME_ActorFeatureName;

	//~ Begin of IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End of IGameFrameworkInitStateInterface interface

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category = "Survival|Pawn")
	TArray<FName> InputMappings;
	
	// True when player input bindings have been applied, will never be true for non - players
	bool bReadyToBindInputs;
};
