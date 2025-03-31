// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingMenuWidget.h"

#include "Components/UniformGridPanel.h"

void UCraftingMenuWidget::CreateAllItems()
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

void UCraftingMenuWidget::OnGridItemClicked(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass)
	{
		
	}
}

void UCraftingMenuWidget::ShowItems()
{
	
}
