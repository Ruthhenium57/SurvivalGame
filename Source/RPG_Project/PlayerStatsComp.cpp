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
		RegenerateStamina(DeltaTime);
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



void UPlayerStatsComp::DecreaseStamina(float DeltaTime)
{
	if (CurrentStamina > 0.0f)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaDecreaseRate * DeltaTime, 0.0f, MaxStamina);

		if (CurrentStamina <= 0.0f)
		{
			OnStaminaEnd.Broadcast();
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
