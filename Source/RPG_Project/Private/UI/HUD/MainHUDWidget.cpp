// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MainHUDWidget.h"
#include "GameFramework/MainPlayerController.h"
#include "UI/Craft/CraftingMenuWidget.h"
#include "UI/HUD/HealthBarWidget.h"
#include "UI/HUD/HungerBarWidget.h"
#include "UI/HUD/StaminaBarWidget.h"
#include "UI/HUD/ThirstBarWidget.h"
#include "UI/Inventory/InventoryWidget.h"

void UMainHUDWidget::InitializeWidgetsProp()
{
	if (!MainPlayerState)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMainHUDWidget::InitializeWidgetsProp, 0.1f, false);
		return;
	}
	UPlayerStatsComponent* PlayerStatsComponent = MainPlayerState->PlayerStatsComponent;
	UInventoryComponent* InventoryComponent = MainPlayerState->InventoryComponent;
	UCraftComponent* CraftComponent = MainPlayerState->CraftComponent;
	HealthBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	StaminaBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	HungerBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	ThirstBarWidget->PlayerStatsComponent = PlayerStatsComponent;
	InventoryWidget->InventoryComponent = InventoryComponent;
	CraftingMenuWidget->InventoryComponent = InventoryComponent;
	CraftingMenuWidget->CraftComponent = CraftComponent;
	CraftingMenuWidget->InitializeAll();
}

void UMainHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AMainPlayerController* Controller = Cast<AMainPlayerController>(GetOwningPlayer());
	if (Controller)
	{
		Controller->OnToggleInventory.AddUObject(this, &UMainHUDWidget::ToggleInventory);
		Controller->OnToggleCraftMenu.AddUObject(this, &UMainHUDWidget::ToggleCraftMenu);
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
