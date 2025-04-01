// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingMenuWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"

void UCraftingMenuWidget::NativeConstruct()
{
	Categories.AddUnique(EItemType::None);
	Categories.AddUnique(EItemType::Component);
	Categories.AddUnique(EItemType::Tool);
	Categories.AddUnique(EItemType::Medicine);
	Categories.AddUnique(EItemType::Object);
	Categories.AddUnique(EItemType::Food);
	Categories.AddUnique(EItemType::Weapon);

	InitializeAllItems();
	InitializeCategories();
	SelectCategory(EItemType::None);
}

void UCraftingMenuWidget::InitializeAllItems()
{
	for (const auto& Pair : OwningPlayer->CraftComponent->CraftDataCache)
	{
		FItemData Data = OwningPlayer->InventoryComponent->GetItemDataFromTable(Pair.Key);
		if (UItemImageSlotWidget* Widget = CreateWidget<UItemImageSlotWidget>(this, UItemImageSlotWidget::StaticClass(), FName("GridItemToCraft")))
		{
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
	for (EItemType Category : Categories)
	{
		if (UCategoryButtonWidget* Widget = CreateWidget<UCategoryButtonWidget>(this, UCategoryButtonWidget::StaticClass(), FName("Category")))
		{
			CategoriesBox->AddChild(Widget);
			Widget->ItemCategory(Category);
			Widget->UpdateWidgetData();
			Widget->OnCategorySelected.AddDynamic(this, &UCraftingMenuWidget::SelectCategory);
		}
	}
}

void UCraftingMenuWidget::OnGridItemClicked(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		
	}
}

void UCraftingMenuWidget::FilterGridByCategory(const EItemType Category)
{
	for (TMap<TSubclassOf<AMainItemActor>, UItemImageSlotWidget*> GridItem : GridItemsCache)

}

void UCraftingMenuWidget::SelectCategory(const EItemType Category)
{
	if (Category == CurrentCategory) return;

}