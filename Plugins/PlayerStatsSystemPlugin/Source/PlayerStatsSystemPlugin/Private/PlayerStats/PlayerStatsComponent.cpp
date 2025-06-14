// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats/PlayerStatsComponent.h"
#include "PlayerStatsLogMacros.h"
#include "PlayerStatsSystemPlugin.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	// Current, Max
	Stats.Add({60.f, 100.f, EPlayerStatType::Health});
	Stats.Add({100.f, 100.f, EPlayerStatType::Stamina});
	Stats.Add({0.f, 100.f, EPlayerStatType::Armor});
	Stats.Add({80.f, 100.f, EPlayerStatType::Thirst});
	Stats.Add({80.f, 100.f, EPlayerStatType::Hunger});


	StaminaRegenRate = 10.0f;
	StaminaDecreaseRate = 15.0f;
	StaminaRegenRateWhenDebuffed = StaminaRegenRate / 2.0f;
	StaminaRegenDelay = 1.5f;
	bCanStaminaRegen = true;
	bIsStaminaDecreasing = false;

	ThirstDecreaseRate = 0.3f;
	HealthDecreaseRateWhenThirstIs0 = 1.0f;
	HungerDecreaseRate = 0.5f;
	HealthDecreaseRateWhenHungerIs0 = 1.25f;
	StaminaRegenDelay = 2.0f;
	bCanStaminaRegen = true;
}

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_Authority)
	{
		TickStamina(DeltaTime);
		TickHunger(DeltaTime);
		TickThirst(DeltaTime);
		TickHealthDebuffs(DeltaTime);
	}
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, Stats, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, bIsStaminaDecreasing, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, bCanStaminaRegen, COND_OwnerOnly);
}

void UPlayerStatsComponent::OnRep_Stats()
{
	TArray<FStatData> TempPreviousStats = PreviousStats;
	PreviousStats = Stats;

	for (const auto& Pair : Stats)
	{
		const EPlayerStatType CurrentType = *Pair.StatType;
		const FStatData& CurrentData = Pair;

		for (const FStatData& PrevDataPair : TempPreviousStats)
		{
			if (PrevDataPair.StatType == CurrentType)
			{
				const FStatData& PrevData = Pair;
				if (CurrentData.MaxValue != PrevData.MaxValue)
				{
					OnMaxStatValueChanged.Broadcast(CurrentType, CurrentData.MaxValue);
				}

				if (CurrentData.CurrentValue != PrevData.CurrentValue)
				{
					OnMaxStatValueChanged.Broadcast(CurrentType, CurrentData.CurrentValue);
				}

				if (CurrentType == EPlayerStatType::Stamina)
				{
					if (CurrentData.CurrentValue <= 0.f && PrevData.CurrentValue > 0)
					{
						OnStaminaEnd.Broadcast(true);
					}
					else if (CurrentData.CurrentValue > 0 && PrevData.CurrentValue <= 0)
					{
						OnStaminaEnd.Broadcast(false);
					}
				}
				return;
			}
		}
		
		{
			OnMaxStatValueChanged.Broadcast(CurrentType, CurrentData.MaxValue);
			OnStatValueChanged.Broadcast(CurrentType, CurrentData.CurrentValue, 0.f, CurrentData.MaxValue);
		}
	}
}

float UPlayerStatsComponent::GetCurrentValue(EPlayerStatType StatType)
{
	if (const FStatData* StatData = GetDataByType(StatType))
	{
		return StatData->CurrentValue;
	}
	LOGF_PS(Warning, "StatType %d not found", StatType);
	return 0.f;
}

float UPlayerStatsComponent::GetMaxValue(EPlayerStatType StatType)
{
	if (const FStatData* StatData = GetDataByType(StatType))
	{
		return StatData->MaxValue;
	}
	LOGF_PS(Warning, "StatType %d not found", StatType);
	return 0.f;
}

float UPlayerStatsComponent::GetCurrentValueNormalized(EPlayerStatType StatType)
{
	if (const FStatData* StatData = GetDataByType(StatType))
	{
		return (StatData->MaxValue > 0.f) ? (StatData->CurrentValue / StatData->MaxValue) : 0.f;
	}
	LOGF_PS(Warning, "StatType %d not found", StatType);
	return 0.f;
}

bool UPlayerStatsComponent::CanSprint()
{
	return GetCurrentValue(EPlayerStatType::Stamina) > 0;
}

FStatData* UPlayerStatsComponent::GetDataByType(EPlayerStatType StatType)
{
	for (FStatData& Pair : Stats)
	{
		if (Pair.StatType == StatType) return &Pair;
	}
	return nullptr;
}

void UPlayerStatsComponent::ModifyValue(EPlayerStatType StatType, float Delta)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		LOGF_PS(Warning, "Called on client");
		return;
	}
	if (FStatData* StatData = GetDataByType(StatType))
	{
		SetCurrentValueInternal(StatType, StatData->CurrentValue + Delta);
	}
	else
	{
		LOGF_PS(Warning, "StatType %d not found", StatType);
	}
}

void UPlayerStatsComponent::SetMaxValue(EPlayerStatType StatType, float NewValue)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		LOGF_PS(Warning, "Called on client");
		return;
	}
	SetMaxValueInternal(StatType, NewValue);
}

void UPlayerStatsComponent::TakeDamage(float AmountDamage)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (AmountDamage <= 0.f) return;
	// TODO: Armor logic
	ModifyValue(EPlayerStatType::Health, -AmountDamage);
}

void UPlayerStatsComponent::CheckDeath()
{
	LOGF_PS(Warning, "Death");
	// TODO: Death logic
}

bool UPlayerStatsComponent::SetCurrentValueInternal(EPlayerStatType StatType, float NewValue)
{
	if (FStatData* StatData = GetDataByType(StatType))
	{
		float OldValue = StatData->CurrentValue;
		float ClampedValue = FMath::Clamp(NewValue, 0.f, StatData->MaxValue);

		if (!FMath::IsNearlyEqual(OldValue, ClampedValue))
		{
			StatData->CurrentValue = ClampedValue;
			OnStatValueChanged.Broadcast(StatType, StatData->CurrentValue, OldValue, StatData->MaxValue);

			if (StatType == EPlayerStatType::Stamina)
			{
				if (StatData->CurrentValue <= 0.f && OldValue > 0)
				{
					OnStaminaEnd.Broadcast(true);
				}
				else if (StatData->CurrentValue > 0 && OldValue <= 0)
				{
					OnStaminaEnd.Broadcast(false);
				}
			}

			if (StatType == EPlayerStatType::Health)
			{
				if (StatData->CurrentValue <= 0 && OldValue > 0)
				{
					CheckDeath();
				}
			}

			if (GetOwnerRole() == ROLE_Authority && GetNetMode() != NM_Standalone)
			{
				// mark dirty maybe
			}

			return true;
		}
	}
	else
	{
		LOGF_PS(Warning, "StatType %d not found", StatType);
	}
	return false;
}

bool UPlayerStatsComponent::SetMaxValueInternal(EPlayerStatType StatType, float NewValue)
{
	if (FStatData* StatData = GetDataByType(StatType))
	{
		float OldValue = StatData->CurrentValue;
		float ClampedValue = FMath::Max(0.f, NewValue);

		if (!FMath::IsNearlyEqual(OldValue, ClampedValue))
		{
			StatData->MaxValue = ClampedValue;
			StatData->CurrentValue = FMath::Min(StatData->CurrentValue, StatData->MaxValue);

			OnMaxStatValueChanged.Broadcast(StatType, ClampedValue);
		}
	}
	else
	{
		LOGF_PS(Warning, "StatType %d not found", StatType);
	}
	return false;
}

void UPlayerStatsComponent::TickStamina(float DeltaTime)
{
	if (bIsStaminaDecreasing)
	{
		ModifyValue(EPlayerStatType::Stamina, -StaminaDecreaseRate * DeltaTime);
	}
	else if (bCanStaminaRegen && GetCurrentValue(EPlayerStatType::Stamina) < GetMaxValue(EPlayerStatType::Stamina))
	{
		bool bIsDebuffed = GetCurrentValue(EPlayerStatType::Hunger) <= 0.f || GetCurrentValue(EPlayerStatType::Thirst)
			<= 0.f;
		float CurrentRegenRate = bIsDebuffed ? StaminaRegenRateWhenDebuffed : StaminaRegenRate;
		ModifyValue(EPlayerStatType::Stamina, CurrentRegenRate * DeltaTime);
	}
}

void UPlayerStatsComponent::TickThirst(float DeltaTime)
{
	ModifyValue(EPlayerStatType::Thirst, -ThirstDecreaseRate * DeltaTime);
}

void UPlayerStatsComponent::TickHunger(float DeltaTime)
{
	ModifyValue(EPlayerStatType::Hunger, -HungerDecreaseRate * DeltaTime);
}

void UPlayerStatsComponent::TickHealthDebuffs(float DeltaTime)
{
	if (GetCurrentValue(EPlayerStatType::Hunger) <= 0.f)
	{
		ModifyValue(EPlayerStatType::Health, -HealthDecreaseRateWhenHungerIs0 * DeltaTime);
	}
	if (GetCurrentValue(EPlayerStatType::Thirst) <= 0.f)
	{
		ModifyValue(EPlayerStatType::Health, -HealthDecreaseRateWhenThirstIs0 * DeltaTime);
	}
}

void UPlayerStatsComponent::StartRegenStamina()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	bCanStaminaRegen = true;
}

void UPlayerStatsComponent::NotifySprintStarted()
{
	if (GetOwnerRole() != ROLE_Authority) return;

	bIsStaminaDecreasing = true;
	bCanStaminaRegen = false;
	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenDelayTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenDelayTimer);
	}
}

void UPlayerStatsComponent::NotifySprintStopped()
{
	if (GetOwnerRole() != ROLE_Authority) return;

	bIsStaminaDecreasing = false;
	GetWorld()->GetTimerManager().SetTimer(StaminaRegenDelayTimer, this, &UPlayerStatsComponent::StartRegenStamina,
	                                       StaminaRegenDelay, false);
}
