// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemImageSlotWidget.generated.h"

class UImage;
class UButton;
class AMainItemActor;
struct FItemData;

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

	UPROPERTY(BlueprintAssignable)
	FOnClickedOnItemImage OnClickedDelegate;

	UPROPERTY()
	TSubclassOf<AMainItemActor> ItemClass;
	
protected:
	UFUNCTION()
	void OnButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
};
