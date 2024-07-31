// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUsedActor.h"

AItemUsedActor::AItemUsedActor()
{
	ItemLogic = CreateDefaultSubobject<UItemUsed>(TEXT("ItemUsedLogic"));
}

void AItemUsedActor::Use()
{
	if (UItemUsed* ItemUsed = Cast<UItemUsed>(ItemLogic))
	{
		ItemUsed->Use();
	}
}
