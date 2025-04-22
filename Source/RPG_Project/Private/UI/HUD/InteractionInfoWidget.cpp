// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InteractionInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/HUD/InteractInfoItemWidget.h"

void UInteractionInfoWidget::UpdateInteractContainer(const TArray<EInteractType>& Interacts,
                                                     const TMap<EInteractType, FKey>& Bindings, FName ObjectName)
{
	InteractionInfoBox->ClearChildren();
	if (Interacts.IsEmpty() || !InteractInfoItemWidgetClass) return;
	ItemName->SetText(FText::FromName(ObjectName));
	for (EInteractType InteractType : Interacts)
	{
		UInteractInfoItemWidget* InfoItemWidget = CreateWidget<UInteractInfoItemWidget>(
			this, InteractInfoItemWidgetClass, TEXT("InteractType"));
		if (InfoItemWidget)
		{
			const FKey* FoundKey = Bindings.Find(InteractType);
			InfoItemWidget->UpdateInteractInfo(InteractType, FoundKey ? *FoundKey : FKey());
			InteractionInfoBox->AddChild(InfoItemWidget);
		}
	}
}
