// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUsed.h"

void UItemUsed::Use()
{
	UE_LOG(LogTemp, Display, TEXT("Item used"));
}

bool UItemUsed::Interact(ACharacter* Character)
{
	Super::Interact(Character);
	return true;
}
