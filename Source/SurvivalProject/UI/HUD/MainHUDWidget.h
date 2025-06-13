// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UCraftingMenuWidget;
class UInventoryWidget;
class UPlayerStatBarWidget;
/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void InitializeWidgetsProp();
	virtual void NativeConstruct() override;
	
public:	
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPlayerStatBarWidget> HealthBarWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPlayerStatBarWidget> StaminaBarWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPlayerStatBarWidget> ThirstBarWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPlayerStatBarWidget> HungerBarWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInventoryWidget> InventoryWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCraftingMenuWidget> CraftingMenuWidget;

protected:
	UFUNCTION()
	void ToggleInventory();
	UFUNCTION()
	void ToggleCraftMenu();
};
