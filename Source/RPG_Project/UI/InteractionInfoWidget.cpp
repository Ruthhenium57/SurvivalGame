// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionInfoWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UInteractionInfoWidget::ShowInteractInfo(FString NameTextBlock)
{
	for (UWidget* ChildWidget : InteractionInfoBox->GetAllChildren())
	{
		UTextBlock* ChildTextBlock = Cast<UTextBlock>(ChildWidget);
		if (ChildTextBlock && ChildTextBlock->GetName() == NameTextBlock)
		{
			ChildTextBlock->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UInteractionInfoWidget::HideInteractInfo()
{
	for (UWidget* ChildWidget : InteractionInfoBox->GetAllChildren())
	{
		UTextBlock* ChildTextBlock = Cast<UTextBlock>(ChildWidget);
		if (ChildTextBlock)
		{
			ChildTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
