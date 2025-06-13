// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemImageSlotWidget.generated.h"

class UImage;
class UButton;
struct FItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedOnItemImage, int32, ItemClass);
/**
 * 
 */
UCLASS()
class CRAFTSYSTEMPLUGIN_API UItemImageSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateImageInfo(FItemData ItemData);

	UPROPERTY(BlueprintAssignable)
	FOnClickedOnItemImage OnClickedDelegate;

	UPROPERTY()
	int32 ItemID;
	
protected:
	UFUNCTION()
	void OnButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button;
};
