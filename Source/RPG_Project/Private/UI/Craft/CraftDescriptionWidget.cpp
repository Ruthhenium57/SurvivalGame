// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/CraftDescriptionWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Craft/CraftComponent.h"
#include "Craft/CraftData.h"
#include "Inventory/InventoryComponent.h"
#include "UI/Craft/ItemRecipeWidget.h"
#include "GameFramework/PlayerState.h"


void UCraftDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InventoryComponent = GetOwningPlayerState()->GetComponentByClass<UInventoryComponent>();
	CraftComponent = GetOwningPlayerState()->GetComponentByClass<UCraftComponent>();
}

void UCraftDescriptionWidget::SubscribeToInventoryUpdated()
{
	InventoryComponent->OnInventoryChanged.AddUObject(this, &UCraftDescriptionWidget::UpdateDescription);
}

void UCraftDescriptionWidget::OnButtonClicked()
{
	if (bCanCraftItem)
	{
		CraftComponent->CraftItem(ItemData.ItemClass);
	}
}

void UCraftDescriptionWidget::UpdateDescription()
{
	ItemName->SetText(FText::FromName(ItemData.ItemName));
	ItemDescription->SetText(FText::FromName(ItemData.ItemDescription));
	ItemImage = ItemData.ItemImage;
	const FCraftData CraftData = CraftComponent->GetCraftItemData(ItemData.ItemClass);
	bCanCraftItem = true;
	for (const auto& Material : CraftData.CraftMaterials)
	{
		const auto Items = InventoryComponent->FindSlotByClass(ItemData.ItemClass);
		int32 AvailableMaterials = 0;
		if (Items) AvailableMaterials = Items->Quantity;
		UItemRecipeWidget* Widget = CreateWidget<UItemRecipeWidget>(this, ItemRecipeWidgetClass, FName("ItemRecipeWidget"));
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
