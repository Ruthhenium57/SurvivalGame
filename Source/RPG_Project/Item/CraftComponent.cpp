// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MainItemActor.h"
#include "../CraftData.h"


// Sets default values for this component's properties
UCraftComponent::UCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CraftDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Craft.DT_Craft"));
	InventoryComponent = Cast<UInventoryComponent>(GetOwner());
}


// Called when the game starts
void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void UCraftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCraftComponent::CraftItemInternal(TSubclassOf<AMainItemActor> ItemClass)
{
	if (CraftDataTable)
	{
		FCraftData* Data = CraftDataTable->FindRow<FCraftData>(FName(ItemClass->GetName().RightChop(7).LeftChop(2)), TEXT("")); // Find needed row
		if (Data)
		{
			if (InventoryComponent)
			{
				if (InventoryComponent->HowMuchFreeSpaceInSlot(ItemClass) >= Data->Quantity) // If have free space to new item
				{
					for (int32 i = 0; i < Data->Materials.Num(); i++) // loop for materials
					{
						FItemInventorySlot Slot;
						if (InventoryComponent->FindSlotByClass(Data->Materials[i].ItemClass, Slot))
						{
							if (Slot.Items.Num() < Data->Materials[i].Quantity) // if have enough materials to claft
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}
					for (int32 i = 0; i < Data->Materials.Num(); i++)
					{
						
						InventoryComponent->RemoveItem(AMain)
						Data->Materials[i].ItemClass.
					}
				}
			}
		}
	}
}

