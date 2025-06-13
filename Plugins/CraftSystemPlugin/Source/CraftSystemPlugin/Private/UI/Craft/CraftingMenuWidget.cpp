// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Craft/CraftingMenuWidget.h"
#include "CraftDataSubsystem.h"
#include "CraftLogMacros.h"
#include "CraftSystemPlugin.h"
#include "UI/Craft/CategoryButtonWidget.h"
#include "UI/Craft/CraftDescriptionWidget.h"
#include "UI/Craft/ItemImageSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "InventoryDataSubsystem.h"


void UCraftingMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Categories = {
		EItemType::Component, EItemType::Food, EItemType::Medicine, EItemType::Object, EItemType::Tool,
		EItemType::Weapon, EItemType::AllTypes
	};
	InventorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UInventoryDataSubsystem>();
	CraftSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCraftDataSubsystem>();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCraftingMenuWidget::InitializeAll, 0.1f, false);
}

void UCraftingMenuWidget::InitializeAll()
{
	if (CraftComponent && CategoryButtonWidgetClass && ItemImageSlotWidgetClass)
	{
		InitializeAllItems();
		InitializeCategories();
		SelectCategory(EItemType::AllTypes);
	}
}

void UCraftingMenuWidget::InitializeAllItems()
{
	LOGF_CRAFT(Warning, "Try to initialize craft items")
	// cycle through all crafts
	if (TMap<int32, FCraftData>* CraftDataTableCache = CraftSubsystem->GetCachedDataTable(ECraftType::Hand))
	{
		for (const auto& Pair : *CraftDataTableCache)
		{
			FItemData Data = InventorySubsystem->GetItemDataByID(Pair.Key);
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
			Widget->Category = Category;
			Widget->UpdateWidgetData();
			Widget->OnCategorySelected.AddDynamic(this, &UCraftingMenuWidget::SelectCategory);
		}
	}
}

void UCraftingMenuWidget::OnGridItemClicked(int32 ItemClass)
{
	if (ItemClass)
	{
		UCraftDescriptionWidget* Widget = CreateWidget<UCraftDescriptionWidget>(
			this, CraftDescriptionWidgetClass, FName("Description"));
		if (!Widget) return;
		Widget->ItemData = InventorySubsystem->GetItemDataByID(ItemClass);
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
			if (InventorySubsystem->GetItemDataByID(GridItem.Key).ItemType == Category)
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
