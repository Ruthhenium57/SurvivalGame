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
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSprintEndDelegate OnStaminaEnd;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float MaxArmor;
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float CurrentArmor;

	void ChangeHealth(float Amount);
	
	void TakeDamage(float AmountDamage);
	
	void Death();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopSprint();
};




