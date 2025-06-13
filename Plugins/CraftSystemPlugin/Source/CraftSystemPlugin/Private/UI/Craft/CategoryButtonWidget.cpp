// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/CategoryButtonWidget.h"
#include "Components/TextBlock.h"
#include "Inventory/ItemData.h"


void UCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
    Category = EItemType::Component;
}

void UCategoryButtonWidget::OnButtonClicked()
{
    OnCategorySelected.Broadcast(Category);
}

void UCategoryButtonWidget::UpdateWidgetData()
{
	ItemCategoryName->SetText(FText::FromString(StaticEnum<EItemType>()->GetNameStringByValue(static_cast<int64>(Category))));
}