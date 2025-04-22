// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/MainPlayerState.h"
#include "Inventory/InventoryComponent.h"
#include "Craft/CraftComponent.h"
#include "PlayerStats/PlayerStatsComponent.h"


AMainPlayerState::AMainPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	CraftComponent = CreateDefaultSubobject<UCraftComponent>("CraftComponent");
	PlayerStatsComponent = CreateDefaultSubobject<UPlayerStatsComponent>("PlayerStatsComponent");
}
