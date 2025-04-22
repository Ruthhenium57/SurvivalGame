// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

class UCraftComponent;
class UPlayerStatsComponent;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

	AMainPlayerState();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCraftComponent> CraftComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerStatsComponent> PlayerStatsComponent;
};
