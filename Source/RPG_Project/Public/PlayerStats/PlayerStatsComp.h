// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStatsComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintEndDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_PROJECT_API UPlayerStatsComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerStatsComp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifVisibleetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartSprint();
	void StopSprint();
	
private:
	void RegenerateStamina(float DeltaTime);
	void DecreaseStamina(float DeltaTime);
	bool bIsSprinting;

public:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaDecreaseRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRateWhenHungerOrThirstIs0;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSprintEndDelegate OnStaminaEnd;
	bool bCanStaminaRegen;
	void OneTimeStaminaReduction(float AmountStamina);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float CurrentThirst;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float MaxThirst;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float ThirstDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float HealthDecreaseRateWhenThirstIs0;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float CurrentHunger;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float MaxHunger;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungerDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HealthDecreaseRateWhenHungerIs0;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float MaxArmor;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float CurrentArmor;

	void ChangeHealth(float Amount);
	void ChangeThirst(float Amount);
	void ChangeHunger(float Amount);
	
	void TakeDamage(float AmountDamage);
	
	void Death();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOneTimeStaminaReduction(float AmountStamina);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOneTimeStaminaReduction(float AmountStamina);

	void TickDecreaseThirst(float DeltaTime);

	void TickDecreaseHunger(float DeltaTime);

	void TickDecreaseHealth(float DeltaTime, float AmountHealth);
};




