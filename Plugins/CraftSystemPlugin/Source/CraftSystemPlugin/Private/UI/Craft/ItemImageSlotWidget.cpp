// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/ItemImageSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Inventory/ItemData.h"

void UItemImageSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddDynamic(this, &UItemImageSlotWidget::OnButtonClicked);
}

void UItemImageSlotWidget::UpdateImageInfo(const FItemData ItemData)
{
	if (ItemData.ItemImage)
	{
		UTexture2D* LoadedTexture = ItemData.ItemImage.LoadSynchronous();
		if (LoadedTexture)
		{
			ItemImage->SetBrushFromTexture(LoadedTexture);
		}
	}
}

void UItemImageSlotWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(ItemID);
}

