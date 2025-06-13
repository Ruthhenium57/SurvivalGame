// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MainHUDWidget.h"

#include "Characters/SurvivalCharacter.h"
#include "GameFramework/SurvivalPlayerController.h"
#include "GameFramework/SurvivalPlayerState.h"
#include "PlayerStats/PlayerStatType.h"
#include "UI/Craft/CraftingMenuWidget.h"
#include "UI/PlayerStatBarWidget.h"
#include "UI/Inventory/InventoryWidget.h"

void UMainHUDWidget::InitializeWidgetsProp()
{
	ASurvivalPlayerState* MainPlayerState = GetOwningPlayerState<ASurvivalPlayerState>();
	if (!MainPlayerState)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMainHUDWidget::InitializeWidgetsProp, 0.1f, false);
		return;
	}
	// UPlayerStatsComponent* PlayerStatsComponent = MainPlayerState->GetPawn<ASurvivalCharacter>()->GetPlayerStatsComponent();
	// UInventoryComponent* InventoryComponent = MainPlayerState->GetInventoryComponent();
	// UCraftComponent* CraftComponent = MainPlayerState->GetCraftComponent();
	// HealthBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	// HealthBarWidget->PlayerStatType = EPlayerStatType::Health;
	// StaminaBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	// StaminaBarWidget->PlayerStatType = EPlayerStatType::Stamina;
	// HungerBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	// HungerBarWidget->PlayerStatType = EPlayerStatType::Hunger;
	// ThirstBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	// ThirstBarWidget->PlayerStatType = EPlayerStatType::Thirst;
	// InventoryWidget->InventoryComponent = InventoryComponent;
	// CraftingMenuWidget->InventoryComponent = InventoryComponent;
	// CraftingMenuWidget->CraftComponent = CraftComponent;
	// CraftingMenuWidget->InitializeAll();
}

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ASurvivalPlayerController* Controller = Cast<ASurvivalPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		// Controller->OnToggleInventory.AddUObject(this, &UMainHUDWidget::ToggleInventory);
		// Controller->OnToggleCraftMenu.AddUObject(this, &UMainHUDWidget::ToggleCraftMenu);
	}

	InitializeWidgetsProp();
}

void UMainHUDWidget::ToggleInventory()
{
	if (InventoryWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHUDWidget::ToggleCraftMenu()
{
	if (CraftingMenuWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		CraftingMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CraftingMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
