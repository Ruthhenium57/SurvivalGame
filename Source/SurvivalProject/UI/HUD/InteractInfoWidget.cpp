// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InteractInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/SurvivalPlayerController.h"
#include "UI/HUD/InteractKeyInfoWidget.h"

void UInteractInfoWidget::UpdateInteractContainer(
	TMap<EInteractType, TArray<FKey>> Bindings, FName ObjectName)
{
	InteractionInfoBox->ClearChildren();
	if (Bindings.IsEmpty() || !InteractInfoItemWidgetClass) return;
	ItemName->SetText(FText::FromName(ObjectName));

	for (auto Pair : Bindings)
	{
		UInteractKeyInfoWidget* InfoItemWidget = CreateWidget<UInteractKeyInfoWidget>(
			this, InteractInfoItemWidgetClass, TEXT("InteractType"));
		if (InfoItemWidget)
		{
			InfoItemWidget->UpdateInteractInfo(Pair.Key, Pair.Value);
			InteractionInfoBox->AddChild(InfoItemWidget);
		}
	}
}
