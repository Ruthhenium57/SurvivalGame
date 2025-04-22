// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/ItemRecipeWidget.h"

#include "Components/Image.h"
#include "Inventory/ItemData.h"
#include "Components/TextBlock.h"

void UItemRecipeWidget::UpdateInfo(FItemData ItemData, FText ItemRecipeText)
{
	ItemImage->SetBrushFromTexture(ItemData.ItemImage);
	NeededItemsText->SetText(ItemRecipeText);
}
