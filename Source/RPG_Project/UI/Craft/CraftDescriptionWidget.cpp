// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftDescriptionWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "RPG_Project/ItemData.h"
#include "RPG_Project/PlayableCharacter.h"

void UCraftDescriptionWidget::SubscribeToInventoryUpdated()
{
	PlayableCharacter->InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UCraftDescriptionWidget::UpdateDescription);
}

void UCraftDescriptionWidget::OnButtonClicked()
{
	if (bCanCraftItem)
	{
		PlayableCharacter->CraftComponent->CraftItem(ItemData.ItemClass);
	}
}

void UCraftDescriptionWidget::UpdateDescription()
{
	ItemName->SetText(FText::FromString(ItemData.ItemName));
	ItemDescription->SetText(FText::FromString(ItemData.ItemDescription));
	ItemImage = ItemData.ItemImage;
	const FCraftData CraftData = PlayableCharacter->CraftComponent->GetCraftItemData(ItemData.ItemClass);
	bCanCraftItem = true;
	for (const auto& Material : CraftData.Materials)
	{
		const auto [Items] = PlayableCharacter->InventoryComponent->FindSlotByClass(ItemData.ItemClass);
		int32 AvailableMaterials = 0;
		if (!Items.IsEmpty()) AvailableMaterials = Items.Num();
		UItemRecipeWidget* Widget = CreateWidget<UItemRecipeWidget>(this, UItemRecipeWidget::StaticClass(), FName("ItemRecipeWidget"));
		Widget->UpdateInfo(ItemData, FText::FromString(FString::Printf(TEXT("%d / %d"), AvailableMaterials, Material.Quantity)));
		RecipeWidgetBox->AddChild(Widget);
		Widget->SetPadding(FMargin(5.f, 0.f));
		if (AvailableMaterials < Material.Quantity)
		{
			bCanCraftItem = false;
			Widget->NeededItemsText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else Widget->NeededItemsText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	if (bCanCraftItem) CraftButton->SetIsEnabled(true);
	else CraftButton->SetIsEnabled(false);
}
