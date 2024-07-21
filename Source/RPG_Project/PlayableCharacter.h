// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerStatsComp.h"
#include "PlayableCharacter.generated.h"

class UMainHUDWidget;
class UPlayerStatsComp;

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

public:

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;
	

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float StaminaDecreaseRate;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPlayerStatsComp* PlayerStatsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	UMainHUDWidget* MainHUDWidget;

	float DefaultWalkSpeed;
	float SprintSpeed;
	bool bIsSprinting;
};
