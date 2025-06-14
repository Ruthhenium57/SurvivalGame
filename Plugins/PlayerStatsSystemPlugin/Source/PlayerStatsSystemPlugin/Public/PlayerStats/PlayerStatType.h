// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatType.generated.h"


UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Stamina UMETA(DisplayName = "Stamina"),
	Armor UMETA(DisplayName = "Armor"),
	Thirst UMETA(DisplayName = "Thirst"),
	Hunger UMETA(DisplayName = "Hunger"),
	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FStatData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat Data")
	float CurrentValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat Data")
	float MaxValue = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat Data")
	TOptional<EPlayerStatType> StatType;
};
