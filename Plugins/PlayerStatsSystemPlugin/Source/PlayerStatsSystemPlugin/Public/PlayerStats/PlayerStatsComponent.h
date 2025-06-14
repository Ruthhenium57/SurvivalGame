// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStats/PlayerStatType.h"
#include "PlayerStatsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaEndDelegate, bool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStatValueChanged, EPlayerStatType, StatType, float, NewValue, float,
                                              OldValuefloat, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxStatValueChanged, EPlayerStatType, StatType, float, NewMaxValue);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYERSTATSSYSTEMPLUGIN_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPlayerStatsComponent();

public:
	UFUNCTION(BlueprintPure, Category = "Player Stats")
	float GetCurrentValue(EPlayerStatType StatType);

	UFUNCTION(BlueprintPure, Category = "Player Stats")
	float GetMaxValue(EPlayerStatType StatType);

	UFUNCTION(BlueprintPure, Category = "Player Stats")
	float GetCurrentValueNormalized(EPlayerStatType StatType);

	UFUNCTION(BlueprintPure, Category = "Player Stats|Stamina")
	bool CanSprint();
	
	FStatData* GetDataByType(EPlayerStatType StatType);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void ModifyValue(EPlayerStatType StatType, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void SetMaxValue(EPlayerStatType StatType, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void TakeDamage(float AmountDamage);

	UPROPERTY(BlueprintAssignable, Category = "Player Stats|Events")
	FOnStatValueChanged OnStatValueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player Stats|Events")
	FOnMaxStatValueChanged OnMaxStatValueChanged;

	FOnStaminaEndDelegate OnStaminaEnd;

	UFUNCTION(BlueprintCallable, Category = "Player Stats|Stamina")
	void NotifySprintStarted();

	UFUNCTION(BlueprintCallable, Category = "Player Stats|Stamina")
	void NotifySprintStopped();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(ReplicatedUsing = OnRep_Stats, EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats|Core")
	TArray<FStatData> Stats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaRegenRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaRegenRateWhenDebuffed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaRegenDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float ThirstDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thirst")
	float HealthDecreaseRateWhenThirstIs0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungerDecreaseRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HealthDecreaseRateWhenHungerIs0;

	UPROPERTY(Replicated)
	bool bCanStaminaRegen;
	UPROPERTY(Replicated)
	bool bIsStaminaDecreasing;
	FTimerHandle StaminaRegenDelayTimer;

private:
	UFUNCTION()
	void OnRep_Stats();

	TArray<FStatData> PreviousStats;

	bool SetCurrentValueInternal(EPlayerStatType StatType, float NewValue);
	bool SetMaxValueInternal(EPlayerStatType StatType, float NewValue);

	void CheckDeath();

	void TickStamina(float DeltaTime);
	void TickThirst(float DeltaTime);
	void TickHunger(float DeltaTime);
	void TickHealthDebuffs(float DeltaTime);
	void StartRegenStamina();
};
