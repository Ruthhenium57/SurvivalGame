// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemImageSlotWidget.h"
#include "Components/Button.h"

void UItemImageSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Button->OnClicked.AddDynamic(this, &UItemImageSlotWidget::OnButtonClicked);
}

void UItemImageSlotWidget::UpdateImageInfo(UTexture2D* Image)
{
	if (Image)
	{
		ItemImage = Image;
	}
}

void UItemImageSlotWidget::OnButtonClicked()
{

}

