// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InteractInfoItemWidget.h"
#include "Components/TextBlock.h"


void UInteractInfoItemWidget::UpdateInteractInfo(EInteractType InteractType, FKey Key)
{
	if (Key.IsValid())
	{
		FString KeyName = Key.ToString();
		FString InteractTypeName = (StaticEnum<EInteractType>()->GetNameStringByValue(static_cast<int64>(InteractType)));
		FString FormattedString = FString::Printf(TEXT("[%s] %s"), *KeyName, *InteractTypeName);
		InteractName->SetText(FText::FromString(FormattedString));
	}
}
