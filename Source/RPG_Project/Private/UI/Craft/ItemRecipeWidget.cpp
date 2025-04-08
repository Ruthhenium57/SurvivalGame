// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/ItemRecipeWidget.h"

#include "Components/TextBlock.h"

void UItemRecipeWidget::UpdateInfo(FItemData ItemData, FText ItemRecipeText)
{
	ItemImage = ItemData.ItemImage;
	NeededItemsText->SetText(ItemRecipeText);
}
