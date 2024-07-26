// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsComp.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UPlayerStatsComp::UPlayerStatsComp()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaRegenRate = 5.0f;  
	StaminaDecreaseRate = 10.0f;
	StaminaRegenRateWhenHungerOrThirstIs0 = StaminaRegenRate / 1.75f;
	bIsSprinting = false;
	bCanStaminaRegen = true;

	MaxHealth = 100.0f;
	CurrentHealth = 50.0f;

	MaxArmor = 100.0f;
	CurrentArmor = 0.0f;

	MaxThirst = 100.0f;
	CurrentThirst = MaxThirst;
	ThirstDecreaseRate = 0.3f;
	HealthDecreaseRateWhenThirstIs0 = 1.0f;

	MaxHunger = 100.0f;
	CurrentHunger = MaxHunger;
	HungerDecreaseRate = 0.5f;
	HealthDecreaseRateWhenHungerIs0 = 1.25f;
}

void UPlayerStatsComp::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerStatsComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsSprinting)
	{
		DecreaseStamina(DeltaTime);
	}
	else
	{
		if (bCanStaminaRegen)
		{
			RegenerateStamina(DeltaTime);
		}
	}

	TickDecreaseHunger(DeltaTime);
	TickDecreaseThirst(DeltaTime);
}

void UPlayerStatsComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerStatsComp, MaxStamina);
	DOREPLIFETIME(UPlayerStatsComp, CurrentStamina);
	DOREPLIFETIME(UPlayerStatsComp, MaxHealth);
	DOREPLIFETIME(UPlayerStatsComp, CurrentHealth);
	DOREPLIFETIME(UPlayerStatsComp, MaxArmor);
	DOREPLIFETIME(UPlayerStatsComp, CurrentArmor);
	DOREPLIFETIME(UPlayerStatsComp, CurrentThirst);
	DOREPLIFETIME(UPlayerStatsComp, MaxThirst);
	DOREPLIFETIME(UPlayerStatsComp, CurrentHunger);
	DOREPLIFETIME(UPlayerStatsComp, MaxHunger);
}

void UPlayerStatsComp::OneTimeStaminaReduction(float AmountStamina)
{
	CurrentStamina = FMath::Clamp(CurrentStamina - AmountStamina, 0.0, MaxStamina);
}

void UPlayerStatsComp::ChangeHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	if (CurrentHealth == 0.0f)
	{
		Death();
	}
}

void UPlayerStatsComp::ChangeThirst(float Amount)
{
	CurrentThirst = FMath::Clamp(CurrentThirst + Amount, 0, MaxThirst);
}

void UPlayerStatsComp::ChangeHunger(float Amount)
{
	CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0, MaxHunger);
}

void UPlayerStatsComp::StartSprint()
{
	bIsSprinting = true;
	bCanStaminaRegen = false;
}

void UPlayerStatsComp::StopSprint()
{
	bIsSprinting = false;
}

void UPlayerStatsComp::TakeDamage(float AmountDamage)
{
	ChangeHealth(-AmountDamage);
}

void UPlayerStatsComp::Death()
{
	if (CurrentArmor > 1000000)
	{
		CurrentHealth = 10;
	}
}

void UPlayerStatsComp::TickDecreaseThirst(float DeltaTime)
{
	if (CurrentThirst > 0)
	{
		CurrentThirst = FMath::Clamp(CurrentThirst - ThirstDecreaseRate * DeltaTime, 0, MaxThirst);
	}
	else
	{
		TickDecreaseHealth(DeltaTime, HealthDecreaseRateWhenThirstIs0);
	}
}

void UPlayerStatsComp::TickDecreaseHunger(float DeltaTime)
{
	if (CurrentHunger > 0)
	{
		CurrentHunger = FMath::Clamp(CurrentHunger - HungerDecreaseRate * DeltaTime, 0, MaxHunger);
	}
	else
	{
		TickDecreaseHealth(DeltaTime, HealthDecreaseRateWhenHungerIs0);
	}
}

void UPlayerStatsComp::TickDecreaseHealth(float DeltaTime, float AmountHealth)
{
	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - AmountHealth * DeltaTime, 0, MaxHealth);
	}
	else
	{
		Death();
	}
}

void UPlayerStatsComp::ServerOneTimeStaminaReduction_Implementation(float AmountStamina)
{
	OneTimeStaminaReduction(AmountStamina);
	MulticastOneTimeStaminaReduction(AmountStamina);
}

bool UPlayerStatsComp::ServerOneTimeStaminaReduction_Validate(float AmountStamina)
{
	return true;
}

void UPlayerStatsComp::MulticastOneTimeStaminaReduction_Implementation(float AmountStamina)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		OneTimeStaminaReduction(AmountStamina);
	}
}

void UPlayerStatsComp::MulticastStopSprint_Implementation()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		StopSprint();
	}
}

void UPlayerStatsComp::DecreaseStamina(float DeltaTime)
{
	if (CurrentStamina > 0.0f)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaDecreaseRate * DeltaTime, 0.0f, MaxStamina);

		if (CurrentStamina <= 0.0f)
		{
			OnStaminaEnd.Broadcast();
			MulticastStopSprint();
		}
	}
}

void UPlayerStatsComp::RegenerateStamina(float DeltaTime)
{
	if (CurrentStamina < MaxStamina)
	{
		if (CurrentThirst > 0 && CurrentHunger > 0)
		{
			CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
		}
		else
		{
			CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRateWhenHungerOrThirstIs0 * DeltaTime, 0.0f, MaxStamina);
		}
	}
}