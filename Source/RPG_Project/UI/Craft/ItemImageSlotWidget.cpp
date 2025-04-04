// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemImageSlotWidget.h"
#include "Components/Button.h"

void UItemImageSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddDynamic(this, &UItemImageSlotWidget::OnButtonClicked);
}

void UItemImageSlotWidget::UpdateImageInfo(const FItemData ItemData)
{
	if (ItemData.ItemImage)
	{
		ItemImage = ItemData.ItemImage;
	}
}

void UItemImageSlotWidget::OnButtonClicked()
{
	OnClickedDelegate.Broadcast(ItemClass);
}

