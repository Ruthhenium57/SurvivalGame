// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenroryWidget.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"

void UInvenroryWidget::SetInventory(TArray<AMainItemActor*> Items)
{
	
}

void UInvenroryWidget::AddItemToList(AMainItemActor* Item)
{
    UE_LOG(LogTemp, Warning, TEXT("AddItemToListIsCalled"));
    UTextBlock* TextBlock = NewObject<UTextBlock>(InventoryList);
    if (TextBlock)
    {
        TextBlock->SetText(FText::FromString(Item->ItemName));

        UVerticalBoxSlot* VerticalBoxSlot = InventoryList->AddChildToVerticalBox(TextBlock);
    }
}
