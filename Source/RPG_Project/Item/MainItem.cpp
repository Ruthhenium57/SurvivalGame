// Fill out your copyright notice in the Description page of Project Settings.


#include "MainItem.h"
#include "../PlayableCharacter.h"

bool UMainItem::Interact(ACharacter* Character)
{
	APlayableCharacter* PlayableCharacter = Cast<APlayableCharacter>(Character);
	if (PlayableCharacter)
	{
		if (PlayableCharacter->InventoryComponent->AddItem(this, 1))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
