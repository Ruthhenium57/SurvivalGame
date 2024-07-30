// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItem.h"
#include "../PlayableCharacter.h"

void UMainItem::Interact(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		PlayableCharacter->InventoryComponent->AddItem(this, 1);
	}
}
