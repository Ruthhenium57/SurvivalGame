// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUsedActor.h"

AItemUsedActor::AItemUsedActor()
{
	
}

void AItemUsedActor::CallUse()
{
	if (UItemUsed* ItemUsed = Cast<UItemUsed>(ItemLogic))
	{
		ItemUsed->Use();
	}
}
