// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

enum class EInteractType : uint8;
class UMainHUDWidget;
class UInputAction;
class UInputMappingContext;

struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSprintStateChanged, bool);
DECLARE_MULTICAST_DELEGATE(FOnToggleInventory);
DECLARE_MULTICAST_DELEGATE(FOnToggleCraftMenu);

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

	AMainPlayerController();

public:
	FOnSprintStateChanged OnSprintStateChanged;
	FOnToggleInventory OnToggleInventory;
	FOnToggleCraftMenu OnToggleCraftMenu;

	TMap<const UInputAction*, TArray<FKey>> CacheActionToKeys;
	TMap<EInteractType, FKey> CacheInteractTypeToKey;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TMap<EInteractType, UInputAction*> InteractActions;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void TryToCreateMainHUDWidget();
	void UpdateActionToKeysCache();
	void UpdateInteractTypeToKeyCache();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> CurrentMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SecondInteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> UseItemAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ToggleInventoryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ToggleCraftMenuAction;
	
	UPROPERTY(Replicated)
	bool bCanSprint;
	
	UFUNCTION()
	void OnStaminaEnd(bool StaminaEnd);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartJump();
	void StopJump();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSprint();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSprint();
	
	void StartSprint();
	void StopSprint();
	
	void Interact();
	void SecondInteract();
	void UseItem();

	void ToggleInventory();
	void ToggleCraftMenu();
};

