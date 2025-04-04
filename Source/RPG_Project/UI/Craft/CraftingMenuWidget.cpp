// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingMenuWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "RPG_Project/Item/InventoryComponent.h"

void UCraftingMenuWidget::NativeConstruct()
{
	Categories.AddUnique(EItemType::AllTypes);
	Categories.AddUnique(EItemType::Component);
	Categories.AddUnique(EItemType::Tool);
	Categories.AddUnique(EItemType::Weapon);
	Categories.AddUnique(EItemType::Object);
	Categories.AddUnique(EItemType::Food);
	Categories.AddUnique(EItemType::Medicine);
}

void UCraftingMenuWidget::InitializeAll()
{
	// timer to repeat init if owning player is nullptr
	if (!OwningPlayer) GetWorld()->GetTimerManager().SetTimer(InitializeRestartTimerHandle, this, &UCraftingMenuWidget::InitializeAll, 0.1, false);
	InitializeAllItems();
	InitializeCategories();
	SelectCategory(EItemType::AllTypes);
}

void UCraftingMenuWidget::InitializeAllItems()
{
	// cycle through all crafts
	for (const auto& Pair : OwningPlayer->CraftComponent->CraftDataCache)
	{
		FItemData Data = OwningPlayer->InventoryComponent->GetItemDataFromTable(Pair.Key);
		// create craft slot widget
		if (UItemImageSlotWidget* Widget = CreateWidget<UItemImageSlotWidget>(this, UItemImageSlotWidget::StaticClass(), FName("GridItemToCraft")))
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
	if (Categories.IsEmpty()) return;
	// cycle through all categories
	for (EItemType Category : Categories)
	{
		if (UCategoryButtonWidget* Widget = CreateWidget<UCategoryButtonWidget>(this, UCategoryButtonWidget::StaticClass(), FName("Category")))
		{
			// add to cache & update created widget
			CategoriesBox->AddChild(Widget);
			Widget->ItemCategory = Category;
			Widget->UpdateWidgetData();
			Widget->OnCategorySelected.AddDynamic(this, &UCraftingMenuWidget::SelectCategory);
		}
	}
}

void UCraftingMenuWidget::OnGridItemClicked(const TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		
	}
}

void UCraftingMenuWidget::FilterGridByCategory(const EItemType Category)
{
	// show all item if you clicked on all button
	if (Category == EItemType::AllTypes) for (const auto& GridItem : GridItemsCache)
	{
		GridItem.Value->SetVisibility(ESlateVisibility::Visible);
	}
	// show only the category you clicked on
	else for (const auto& GridItem : GridItemsCache)
	{
		if (OwningPlayer->InventoryComponent->GetItemDataFromTable(GridItem.Key).ItemType == Category)
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
	if (!OwningPlayer) return;
	if (Category == CurrentCategory) return;
	FilterGridByCategory(Category);
	CurrentCategory = Category;
}