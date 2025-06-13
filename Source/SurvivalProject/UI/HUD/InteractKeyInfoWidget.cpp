// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/InteractKeyInfoWidget.h"
#include "Components/TextBlock.h"


void UInteractKeyInfoWidget::UpdateInteractInfo(EInteractType InteractType, TArray<FKey> Keys)
{
	if (!Keys.IsEmpty())
	{
		TArray<FString> KeyNames;
		for (const FKey& Key : Keys)
		{
			KeyNames.Add(Key.GetDisplayName().ToString());
		}
		FString KeysString = FString::Join(KeyNames, TEXT(", "));
		FString InteractTypeName = (StaticEnum<EInteractType>()->GetNameStringByValue(static_cast<int64>(InteractType)));
		FString FormattedString = FString::Printf(TEXT("[%s] %s"), *KeysString, *InteractTypeName);
		InteractName->SetText(FText::FromString(FormattedString));
	}
}
