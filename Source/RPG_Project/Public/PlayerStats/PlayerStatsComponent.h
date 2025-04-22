// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaEndDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnThirstChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxThirstChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHungerChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHungerChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnArmorChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxArmorChangedDelegate, float);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPG_PROJECT_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPlayerStatsComponent();

public:
	void AddHealth(float Amount);
	void AddThirst(float Amount);
	void AddHunger(float Amount);
	void AddStamina(float Amount);
	void TakeDamage(float AmountDamage);
	
	FOnStaminaEndDelegate OnStaminaEnd;
	FOnHealthChangedDelegate OnHealthChanged;
	FOnStaminaChangedDelegate OnStaminaChanged;
	FOnThirstChangedDelegate OnThirstChanged;
	FOnHungerChangedDelegate OnHungerChanged;
	FOnArmorChangedDelegate OnArmorChanged;
	FOnMaxHealthChangedDelegate OnMaxHealthChanged;
	FOnMaxStaminaChangedDelegate OnMaxStaminaChanged;
	FOnMaxThirstChangedDelegate OnMaxThirstChanged;
	FOnMaxHungerChangedDelegate OnMaxHungerChanged;
	FOnMaxArmorChangedDelegate OnMaxArmorChanged;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxStamina, EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina, EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenRateWhenHungerOrThirstIs0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (AllowPrivateAccess = "true"))
	float StaminaRegenDelay;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentThirst, EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float CurrentThirst;
	UPROPERTY(ReplicatedUsing = OnRep_MaxThirst, EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float MaxThirst;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float ThirstDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float HealthDecreaseRateWhenThirstIs0;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHunger, EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float CurrentHunger;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHunger, EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float MaxHunger;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungerDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HealthDecreaseRateWhenHungerIs0;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxArmor, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float MaxArmor;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentArmor, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float CurrentArmor;

private:
	UFUNCTION()
	void OnSprintStateChanged(bool bIsSprinting);
	void StartSprint();
	void StopSprint();
	void StartRegenStamina();
	UPROPERTY(Replicated);
	bool bCanStaminaRegen;
	UPROPERTY(Replicated);
	bool bIsStaminaDecreasing;
	FTimerHandle StaminaRegenDelayTimer;
	
	UFUNCTION()
	void OnRep_MaxStamina();
	UFUNCTION()
	void OnRep_CurrentStamina();

	UFUNCTION()
	void OnRep_MaxHealth();
	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_MaxThirst();
	UFUNCTION()
	void OnRep_CurrentThirst();

	UFUNCTION()
	void OnRep_MaxHunger();
	UFUNCTION()
	void OnRep_CurrentHunger();

	UFUNCTION()
	void OnRep_MaxArmor();
	UFUNCTION()
	void OnRep_CurrentArmor();
	
	void Death();
	
	void TickRegenStamina(float DeltaTime);
	void TickDecreaseStamina(float DeltaTime);
	void TickDecreaseThirst(float DeltaTime);
	void TickDecreaseHunger(float DeltaTime);
	void TickDecreaseHealth(float DeltaTime, float AmountHealth);
};




