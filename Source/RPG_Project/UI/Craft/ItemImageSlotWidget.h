// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainItemActor.h"
#include "ItemImageSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(OnClickedOnItemImage, TSubclassOf<AMainItemActor>, ItemClass);
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
	void UpdateImageInfo(UTexture2D* Image);

	UFUNCTION()
	void OnButtonClicked();
	
	UPROPERTY(BlueprintAssignable)
	FOnClickedOnItemImage OnClickedOnItemImage;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTexture2D* ItemImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

	UPROPERTY()
	TSubclassOf<AMainItemActor> ItemClass;
};
