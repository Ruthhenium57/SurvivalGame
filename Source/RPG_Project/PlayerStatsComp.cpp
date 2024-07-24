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
	bIsSprinting = false;
	bCanStaminaRegen = true;

	MaxHealth = 100.0f;
	CurrentHealth = 50.0f;

	MaxArmor = 100.0f;
	CurrentArmor = 0.0f;
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
}

void UPlayerStatsComp::OneTimeStaminaReduction(float AmountStamina)
{
	CurrentStamina = FMath::Clamp(CurrentStamina - AmountStamina, 0.0, MaxStamina);
}

void UPlayerStatsComp::ChangeHealth(float Amount)
{
	CurrentHealth = CurrentHealth - Amount;
	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		Death();
	}
	if (CurrentHealth >= MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	
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
	ChangeHealth(5.0f);
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
		CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
	}
}