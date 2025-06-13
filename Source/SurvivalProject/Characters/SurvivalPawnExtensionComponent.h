// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "SurvivalPawnExtensionComponent.generated.h"

class UInputMappingContext;
class USurvivalAbilitySystemComponent;
class USurvivalPawnData;

namespace EEndPlayReason { enum Type : int; }

/**
 *	USurvivalPawnExtensionComponent
 *	
 * Component that adds functionality to all Pawn classes so it can be used for characters/vehicles/etc.
 * This coordinates the initialization of other components.
 */
UCLASS()
class SURVIVALPROJECT_API USurvivalPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	USurvivalPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// The name of this overall feature, this one depends on the other named component features 
	static const FName NAME_ActorFeatureName;
	
	//~ Begin of IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End of IGameFrameworkInitStateInterface

	UFUNCTION(BlueprintPure, Category = "Survival|Pawn")
	static USurvivalPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<USurvivalPawnExtensionComponent>() : nullptr;
	}

	// Gets the pawn data, which is used to specify pawn properties in data
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	// Sets the current pawn data
	void SetPawnData(const USurvivalPawnData* InPawnData);
	
	// Gets the current ability system component, which may be owned by a different actor
	UFUNCTION(BlueprintPure, Category = "Survival|Pawn")
	USurvivalAbilitySystemComponent* GetSurvivalAbilitySystemComponent() const { return AbilitySystemComponent; }

	// Should be called by the owning pawn to become the avatar of the ability system
	void InitializeAbilitySystem(USurvivalAbilitySystemComponent* InASC, AActor* InOwnerActor);

	// Should be called by the owning pawn to remove itself as the avatar of the ability system
	void UninitializeAbilitySystem();

	// Should be called by the owning pawn when the pawn's controller changes
	void HandleControllerChanged();

	// Should be called by the owning pawn when the player state has been replicated
	void HandlePlayerStateReplicated();

	// Should be called by the owning pawn when the input component is setup
	void SetupPlayerInputComponent();

	// Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component 
	void OnAbilitySystemInitialized_RegisterAndCall(const FSimpleMulticastDelegate::FDelegate& Delegate);

	// Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor 
	void OnAbilitySystemUninitialized_Register(const FSimpleMulticastDelegate::FDelegate& Delegate);
	
protected:

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnRep_PawnData();

	// Delegate fired when our pawn becomes the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Delegate fired when our pawn is removed as the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
	// Pawn data used to create the pawn. Specified from a spawn function or on a placed instance
	UPROPERTY(EditAnywhere, Category = "Survival|Pawn")
	TObjectPtr<const USurvivalPawnData> PawnData;
	
	// Pointer to the ability system component that is cached for convenience
	UPROPERTY(Transient)
	TObjectPtr<USurvivalAbilitySystemComponent> AbilitySystemComponent;
};
