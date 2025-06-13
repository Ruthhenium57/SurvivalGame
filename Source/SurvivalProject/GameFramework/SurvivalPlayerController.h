// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "SurvivalPlayerController.generated.h"

class UIMCAndPriority;
class USurvivalPawnExtensionComponent;
class USurvivalAbilitySystemComponent;
class ASurvivalPlayerState;

DECLARE_MULTICAST_DELEGATE(FOnToggleInventory);
DECLARE_MULTICAST_DELEGATE(FOnToggleCraftMenu);

/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API ASurvivalPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ASurvivalPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Survival|PlayerController")
	ASurvivalPlayerState* GetSurvivalPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|PlayerController")
	USurvivalAbilitySystemComponent* GetSurvivalAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Survival|PlayerController")
	USurvivalPawnExtensionComponent* GetPawnExtensionComponent() const;

	void AddIMC(const UIMCAndPriority* Mapping) const;
	void RemoveIMC(const UIMCAndPriority* Mapping) const;
	
	//~ Begin of AActor interface
	//virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	// virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of AActor interface

	//~ Begin of AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	// virtual void InitPlayerState() override;
	// virtual void CleanupPlayerState() override;
	// virtual void OnRep_PlayerState() override;
	//~ End of AController interface

	//~APlayerController interface
	//virtual void ReceivedPlayer() override;
	//virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	// virtual void AddCheats(bool bForce) override;
	// virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	// virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	// virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
	
protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

	//void OnSettingsChanged(USurvivalSettingsShared* Settings);
	
private:
	void BroadcastOnPlayerStateChanged();
	
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;
};

