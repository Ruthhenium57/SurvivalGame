// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats/PlayerStatsComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/MainPlayerController.h"
#include "GameFramework/MainPlayerState.h"
#include "Net/UnrealNetwork.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaRegenRate = 5.0f;  
	StaminaDecreaseRate = 10.0f;
	StaminaRegenRateWhenHungerOrThirstIs0 = StaminaRegenRate / 1.75f;
	StaminaRegenDelay = 2.0f;
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

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetOwner());
	if (PlayerController)
	{
		PlayerController->OnSprintStateChanged.AddUObject(this, &UPlayerStatsComponent::OnSprintStateChanged);
	}
}

void UPlayerStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsStaminaDecreasing)
	{
		TickDecreaseStamina(DeltaTime);
	}
	else
	{
		if (bCanStaminaRegen)
		{
			TickRegenStamina(DeltaTime);
		}
	}

	TickDecreaseHunger(DeltaTime);
	TickDecreaseThirst(DeltaTime);
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, MaxStamina, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, CurrentStamina, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, MaxHealth, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, CurrentHealth, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, MaxArmor, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, CurrentArmor, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, CurrentThirst, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, MaxThirst, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, CurrentHunger, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerStatsComponent, MaxHunger, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, bIsStaminaDecreasing, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, bCanStaminaRegen, COND_OwnerOnly);
}

void UPlayerStatsComponent::AddStamina(float AmountStamina)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	CurrentStamina = FMath::Clamp(CurrentStamina - AmountStamina, 0.0, MaxStamina);
}

void UPlayerStatsComponent::AddHealth(float Amount)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	if (CurrentHealth == 0.0f)
	{
		Death();
	}
}

void UPlayerStatsComponent::AddThirst(float Amount)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	CurrentThirst = FMath::Clamp(CurrentThirst + Amount, 0, MaxThirst);
}

void UPlayerStatsComponent::AddHunger(float Amount)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0, MaxHunger);
}

void UPlayerStatsComponent::TakeDamage(float AmountDamage)
{
	AddHealth(-AmountDamage);
}

void UPlayerStatsComponent::Death()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	UE_LOG(LogTemp, Warning, TEXT("You're dead"));
}

void UPlayerStatsComponent::OnSprintStateChanged(bool bIsSprinting)
{
	bIsStaminaDecreasing = bIsSprinting;
	if (bIsSprinting)
	{
		StartSprint();
	}
	else
	{
		StopSprint();
	}
}

void UPlayerStatsComponent::StartSprint()
{
	bCanStaminaRegen = false;
	if (GetOwnerRole() != ROLE_Authority) return;
	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenDelayTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenDelayTimer);
	}
}

void UPlayerStatsComponent::StopSprint()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	GetWorld()->GetTimerManager().SetTimer(StaminaRegenDelayTimer, this, &UPlayerStatsComponent::StartRegenStamina, StaminaRegenDelay, false);
}

void UPlayerStatsComponent::StartRegenStamina()
{
	if (GetOwnerRole() != ROLE_Authority) return;
	bCanStaminaRegen = true;
	OnStaminaEnd.Broadcast(false);
}

void UPlayerStatsComponent::OnRep_MaxStamina()
{
	OnMaxStaminaChanged.Broadcast(MaxStamina);
}

void UPlayerStatsComponent::OnRep_CurrentStamina()
{
	OnStaminaChanged.Broadcast(CurrentStamina);
}

void UPlayerStatsComponent::OnRep_MaxHealth()
{
	OnMaxHealthChanged.Broadcast(MaxHealth);
}

void UPlayerStatsComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth);
}

void UPlayerStatsComponent::OnRep_MaxThirst()
{
	OnMaxThirstChanged.Broadcast(MaxThirst);
}

void UPlayerStatsComponent::OnRep_CurrentThirst()
{
	OnThirstChanged.Broadcast(CurrentThirst);
}

void UPlayerStatsComponent::OnRep_MaxHunger()
{
	OnMaxHungerChanged.Broadcast(MaxHunger);
}

void UPlayerStatsComponent::OnRep_CurrentHunger()
{
	OnHungerChanged.Broadcast(CurrentHunger);
}

void UPlayerStatsComponent::OnRep_MaxArmor()
{
	OnMaxArmorChanged.Broadcast(MaxArmor);
}

void UPlayerStatsComponent::OnRep_CurrentArmor()
{
	OnArmorChanged.Broadcast(CurrentArmor);
}

void UPlayerStatsComponent::TickDecreaseThirst(float DeltaTime)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (CurrentThirst > 0)
	{
		CurrentThirst = FMath::Clamp(CurrentThirst - ThirstDecreaseRate * DeltaTime, 0, MaxThirst);
	}
	else
	{
		TickDecreaseHealth(DeltaTime, HealthDecreaseRateWhenThirstIs0);
	}
}

void UPlayerStatsComponent::TickDecreaseHunger(float DeltaTime)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (CurrentHunger > 0)
	{
		CurrentHunger = FMath::Clamp(CurrentHunger - HungerDecreaseRate * DeltaTime, 0, MaxHunger);
	}
	else
	{
		TickDecreaseHealth(DeltaTime, HealthDecreaseRateWhenHungerIs0);
	}
}

void UPlayerStatsComponent::TickDecreaseHealth(float DeltaTime, float AmountHealth)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (CurrentHealth > 0)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - AmountHealth * DeltaTime, 0, MaxHealth);
	}
	else
	{
		Death();
	}
}

void UPlayerStatsComponent::TickDecreaseStamina(float DeltaTime)
{
	if (GetOwnerRole() != ROLE_Authority) return;
	if (CurrentStamina > 0.0f)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - StaminaDecreaseRate * DeltaTime, 0.0f, MaxStamina);

		if (CurrentStamina <= 0.0f)
		{
			OnStaminaEnd.Broadcast(true);
		}
	}
	else
	{
		OnStaminaEnd.Broadcast(true);
	}
}

void UPlayerStatsComponent::TickRegenStamina(float DeltaTime)
{
	if (GetOwnerRole() != ROLE_Authority) return;
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