// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "SurvivalPlayerState.generated.h"


class USurvivalAbilityTagRelationshipMapping;
class ASurvivalPlayerController;
struct FGameplayTag;
class USurvivalExperienceDefinition;
class USurvivalPawnData;
class USurvivalAbilitySystemComponent;

/**
 *	ASurvivalPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class SURVIVALPROJECT_API ASurvivalPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASurvivalPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "Survival|Gameplay")
	TObjectPtr<const USurvivalPawnData> DefaultPawnData;
	
	UFUNCTION(BlueprintCallable, Category = "Survival|PlayerState")
	ASurvivalPlayerController* GetSurvivalPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|PlayerState")
	USurvivalAbilitySystemComponent* GetSurvivalAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const USurvivalPawnData* InPawnData);

	//~AActor interface
	//virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	//virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	// virtual void CopyProperties(APlayerState* PlayerState) override;
	// virtual void OnDeactivated() override;
	// virtual void OnReactivated() override;
	//~End of APlayerState interface

	static const FName NAME_SurvivalAbilityReady;

	// void SetPlayerConnectionType(ESurvivalPlayerConnectionType NewType);
	// ESurvivalPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }
	
	// // Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	// void AddStatTagStack(FGameplayTag Tag, int32 StackCount);
	//
	// // Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	// void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);
	//
	// // Returns the stack count of the specified tag (or 0 if the tag is not present)
	// UFUNCTION(BlueprintCallable, Category=Teams)
	// int32 GetStatTagStackCount(FGameplayTag Tag) const;
	//
	// // Returns true if there is at least one stack of the specified tag
	// UFUNCTION(BlueprintCallable, Category=Teams)
	// bool HasStatTag(FGameplayTag Tag) const;

	// // Send a message to just this player
	// // (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	// UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Survival|PlayerState")
	// void ClientBroadcastMessage(const FSurvivalVerbMessage Message);
	
private:
	//void OnExperienceLoaded(const USurvivalExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const USurvivalPawnData> PawnData;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Survival|PlayerState")
	TObjectPtr<USurvivalAbilitySystemComponent> AbilitySystemComponent;

	// // Health attribute set used by this actor.
	// UPROPERTY()
	// TObjectPtr<const class USurvivalHealthSet> HealthSet;
	// // Combat attribute set used by this actor.
	// UPROPERTY()
	// TObjectPtr<const class USurvivalCombatSet> CombatSet;
	
	// UPROPERTY(Replicated)
	// ESurvivalPlayerConnectionType MyPlayerConnectionType;
	//
	// UPROPERTY(Replicated)
	// FGameplayTagStackContainer StatTags;
	//
	// UPROPERTY(Replicated)
	// FRotator ReplicatedViewRotation;
};
