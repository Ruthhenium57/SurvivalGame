// Fill out your copyright notice in the Description page of Project Settings.


#include "CategoryButtonWidget.h"

#include "Components/TextBlock.h"

void UCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
    ItemCategory = EItemType::None;
}

void UCategoryButtonWidget::OnButtonClicked()
{
    OnCategorySelected.Broadcast(ItemCategory);
}

void UCategoryButtonWidget::UpdateWidgetData()
{
	CategoryNameBlock->SetText(FText::FromString(StaticEnum<EItemType>()->GetNameStringByValue(static_cast<int32>(ItemCategory))));

    if (ItemCategory == EItemType::Component) SetIcon(TEXT("/Game/"));
    if (ItemCategory == EItemType::Weapon) SetIcon(TEXT("/Game/"));
    if (ItemCategory == EItemType::Tool) SetIcon(TEXT("/Game/"));
}

void UCategoryButtonWidget::SetIcon(const FString& IconPath)
{
    FSoftObjectPath AssetPath(IconPath);
    if (UTexture2D* Icon = Cast<UTexture2D>(AssetPath.TryLoad())) CategoryIcon = Icon;
}