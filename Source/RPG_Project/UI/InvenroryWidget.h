// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Item/MainItemActor.h"
#include "InvenroryWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UInvenroryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetInventory(TArray<AMainItemActor*> Items);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddItemToList(AMainItemActor* Item);

protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* InventoryList;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text;
};
