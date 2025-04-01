// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Item/InventoryComponent.h"
#include "ItemSlotWidget.h"
#include "../PlayableCharacter.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Construct();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateSlotInfo(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateInventory(TArray<FItemInventorySlot> ItemSlots);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddNewSlot(TSubclassOf<AMainItemActor> ItemClass);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveSlot(TSubclassOf<AMainItemActor> ItemClass);

	UPROPERTY()
	APlayableCharacter* OwningPlayer;

protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* InventoryList;

	UPROPERTY()
	TMap<FString, UItemSlotWidget*> ItemSlotWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UItemSlotWidget> ItemWidgetClass;
};
