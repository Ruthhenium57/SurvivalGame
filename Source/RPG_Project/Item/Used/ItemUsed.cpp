// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUsed.h"

void UItemUsed::Use()
{
	UE_LOG(LogTemp, Display, TEXT("Item used"));
}

void UItemUsed::Interact()
{
	Super::Interact();
}
