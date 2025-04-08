// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/MainItemActor.h"
#include "ItemImageSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedOnItemImage, TSubclassOf<AMainItemActor>, ItemClass);
/**
 * 
 */
UCLASS()
class RPG_PROJECT_API UItemImageSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateImageInfo(FItemData ItemData);

	UFUNCTION()
	void OnButtonClicked();
	
	UPROPERTY(BlueprintAssignable)
	FOnClickedOnItemImage OnClickedDelegate;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

	UPROPERTY()
	TSubclassOf<AMainItemActor> ItemClass;
};
