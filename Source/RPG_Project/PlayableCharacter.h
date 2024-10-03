// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Item/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InteractableInterface.h"
#include "PlayerStatsComp.h"
#include "PlayableCharacter.generated.h"

class UMainHUDWidget;
class UPlayerStatsComp;
class UInventoryComponent;

UCLASS()
class RPG_PROJECT_API APlayableCharacter : public AMainCharacter
{
	GENERATED_BODY()

public:
	APlayableCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	float DefaultWalkSpeed;
	float SprintSpeed;
	bool bIsSprinting;

	void PerformLineTrace(FHitResult& HitResult);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UMainHUDWidget* MainHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

	UFUNCTION()
	void InitializeWidget();

	UPROPERTY()
	FTimerHandle TimerHandle_InitWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPlayerStatsComp* PlayerStatsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	virtual void SetupPlayerInputComponent(class UInputComponent* MainPlayerInput) override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Look(float value);
	void Turn(float value);
	void Jump();
	void StopJump();
	UFUNCTION()
	void OnStaminaEnd();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetWalkSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetSprintSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Sprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprintSpeed(float NewSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetSprintSpeed(float NewSpeed);

	UFUNCTION()
	void UpdateHealthBar();
	UFUNCTION()
	void UpdateStaminaBar();
	UFUNCTION()
	void UpdateThirstBar();
	UFUNCTION()
	void UpdateHungerBar();
	UFUNCTION()
	void UpdateInteractInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float NeedStaminaToJump;

	FTimerHandle StaminaRegenTimerHandle;
	float StaminaRegenDelay;
	void BeginStaminaRegen();

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionDistance;
	void Interact();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(AActor* HitActor);

	void PutItemToStorage();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPutItemToStorage(AActor* HitActor);

	UFUNCTION()
	void OnItemAdded(bool bSuccess, AMainItemActor* Item);

	UFUNCTION()
	void OnItemRemoved(bool bSuccess, AMainItemActor* Item);

	UFUNCTION()
	void ToggleInventory();

	bool bIsInventoryHiden;
};