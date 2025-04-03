// Fill out your copyright notice in the Description page of Project Settings.


#include "CategoryButtonWidget.h"
#include "Components/TextBlock.h"

void UCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
    ItemCategory = EItemType::AllTypes;
}

void UCategoryButtonWidget::OnButtonClicked()
{
    OnCategorySelected.Broadcast(ItemCategory);
}

void UCategoryButtonWidget::UpdateWidgetData()
{
	Name->SetText(FText::FromString(StaticEnum<EItemType>()->GetNameStringByValue(static_cast<int32>(ItemCategory))));
}