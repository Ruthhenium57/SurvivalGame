// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/CraftingMenuWidget.h"
#include "UI/Craft/CategoryButtonWidget.h"
#include "UI/Craft/CraftDescriptionWidget.h"
#include "UI/Craft/ItemImageSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Inventory/MainItemActor.h"
#include "InventoryDataSubsystem.h"


void UCraftingMenuWidget::NativeConstruct()
{
	Categories = {
		EItemType::Component, EItemType::Food, EItemType::Medicine, EItemType::Object, EItemType::Tool,
		EItemType::Weapon, EItemType::AllTypes
	};
	InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
}

void UCraftingMenuWidget::InitializeAll()
{
	InitializeAllItems();
	InitializeCategories();
	SelectCategory(EItemType::AllTypes);
}

void UCraftingMenuWidget::InitializeAllItems()
{
	// cycle through all crafts
	for (const auto& Pair : CraftComponent->CraftDataCache)
	{
		FItemData Data = InventorySubsystem->GetItemDataByClass(Pair.Key);
		// create craft slot widget
		if (UItemImageSlotWidget* Widget = CreateWidget<UItemImageSlotWidget>(
			this, ItemImageSlotWidgetClass, FName("GridCraftItem")))
		{
			// add to cache & update created widget 
			CraftsGrid->AddChild(Widget);
			GridItemsCache.Add(Pair.Key, Widget);
			Widget->UpdateImageInfo(Data);
			Widget->OnClickedDelegate.AddDynamic(this, &UCraftingMenuWidget::OnGridItemClicked);
		}
	}
}

void UCraftingMenuWidget::InitializeCategories()
{
	// cycle through all categories
	for (EItemType Category : Categories)
	{
		if (UCategoryButtonWidget* Widget = CreateWidget<UCategoryButtonWidget>(
			this, CategoryButtonWidgetClass, FName("Category")))
		{
			// add to cache & update created widget
			CategoriesBox->AddChild(Widget);
			Widget->ItemCategory = Category;
			Widget->UpdateWidgetData();
			Widget->OnCategorySelected.AddDynamic(this, &UCraftingMenuWidget::SelectCategory);
		}
	}
}

void UCraftingMenuWidget::OnGridItemClicked(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		UCraftDescriptionWidget* Widget = CreateWidget<UCraftDescriptionWidget>(
			this, CraftDescriptionWidgetClass, FName("Description"));
		if (!Widget) return;
		Widget->ItemData = InventorySubsystem->GetItemDataByClass(ItemClass);
	}
}

void UCraftingMenuWidget::FilterGridByCategory(const EItemType Category)
{
	// show all item if you clicked on all button
	if (Category == EItemType::AllTypes)
		for (const auto& GridItem : GridItemsCache)
		{
			GridItem.Value->SetVisibility(ESlateVisibility::Visible);
		}
	// show only the category you clicked on
	else
		for (const auto& GridItem : GridItemsCache)
		{
			if (InventorySubsystem->GetItemDataByClass(GridItem.Key).ItemType == Category)
			{
				GridItem.Value->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				GridItem.Value->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
}

void UCraftingMenuWidget::SelectCategory(const EItemType Category)
{
	if (Category == CurrentCategory) return;
	FilterGridByCategory(Category);
	CurrentCategory = Category;
}
