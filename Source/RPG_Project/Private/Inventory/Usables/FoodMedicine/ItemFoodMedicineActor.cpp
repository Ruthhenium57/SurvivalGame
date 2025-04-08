// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Usables/FoodMedicine/ItemFoodMedicineActor.h"
#include "Characters/PlayerCharacter.h"

AItemFoodMedicineActor::AItemFoodMedicineActor()
{
	ChangeHealth = 0;
	ChangeHunger = 0;
	ChangeThirst = 0;
}

void AItemFoodMedicineActor::Use()
{
	Super::Use();
	ChangeStats();
	Destroy();
}

void AItemFoodMedicineActor::ChangeStats()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
	if (Character)
	{
		UE_LOG(LogTemp, Display, TEXT("ChangeStatsIsSeccesful"));
		Character->PlayerStatsComp->ChangeHealth(ChangeHealth);
		Character->PlayerStatsComp->ChangeHunger(ChangeHunger);
		Character->PlayerStatsComp->ChangeThirst(ChangeThirst);
	}
}
