// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MainItemActor.h"
#include "Engine/DataTable.h"


// Sets default values for this component's properties
UCraftComponent::UCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CraftType = ECraftType::Hand;
}


// Called when the game starts
void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheCraftDT();
}


// Called every frame
void UCraftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCraftComponent::CraftItem(TSubclassOf<AMainItemActor> ItemClass)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		bool bSuccess = CraftItemInternal(ItemClass);
		return bSuccess;
	}
	else
	{
		ServerCraftItem(ItemClass); 
		return false;
	}
}

FCraftData UCraftComponent::GetCraftItemData(TSubclassOf<AMainItemActor> ItemClass)
{
	if (!ItemClass) return FCraftData();
	const FCraftData* CraftData = CraftDataCache.Find(ItemClass);
	if (CraftData) return *CraftData;
	UE_LOG(LogTemp, Warning, TEXT("CraftComponent::GetCraftItemData: No craft item data found for class %s"), *ItemClass->GetName());
	return FCraftData();
}

bool UCraftComponent::CraftItemInternal(TSubclassOf<AMainItemActor> ItemClass)
{
	if (ItemClass) 
	{
		FCraftData Data = GetCraftItemData(ItemClass);
		if (InventoryComponent && Data.ItemToCraft)
		{
			if (InventoryComponent->HowMuchFreeSpaceInSlot(Data.ItemToCraft) >= Data.Quantity) // If have free space to new item
			{
				TArray<TSubclassOf<AMainItemActor>> ChangedKeys;
				for (const auto& Material : Data.Materials) // loop for materials
				{
					FItemInventorySlot Slot = InventoryComponent->FindSlotByClass(Material.ItemClass);  // find material slot 
					if (Slot.Items.Num() < Material.Quantity) // if have enough materials to claft
					{ 
						UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Player has no enough comp of type: %s"), *Material.ItemClass->GetClass()->GetName()); 
						return false; 
					} 
				}
				for (const auto& Material : Data.Materials) // loop for materials type
				{
					for (int32 i = 0; i < Material.Quantity; i++) // loop for material count one type
					{
						InventoryComponent->RemoveItem(nullptr, Data.Materials[i].ItemClass, true); // remove each item
					}
					ChangedKeys.AddUnique(Material.ItemClass);	// add material to changed list
				}
				for (int32 i = 0; i < Data.Quantity; i++) // loop for materials type
				{
					AMainItemActor* ItemToCraft = GetWorld()->SpawnActor<AMainItemActor>(Data.ItemToCraft);  // spawn new item
					InventoryComponent->AddItem(ItemToCraft);	// add new item to inventory
					ItemToCraft->MulticastHideItem();	 // hide new item
					ChangedKeys.AddUnique(ItemToCraft->GetClass()); // add item to changed list
				}
				InventoryComponent->MulticastUpdateSpecificSlots(ChangedKeys);
				UE_LOG(LogTemp, Display, TEXT("UCraftComponent: Item has been crafted"));
				return true;
			}
			else UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Player has no free space"));
		}
		else UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Craft item data is empty"));
	} 
	return false;
}

void UCraftComponent::CacheCraftDT()
{
	if (!CraftDataTable)
	{	// load DataTable depending on ECraftType
		if (CraftType == ECraftType::Hand)
		{
			CraftDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft"));
			if (!CraftDataTable) { UE_LOG(LogTemp, Error, TEXT("UCraftComponent::CachingDT: Failed to load hand craft DT")); return; }
		}
		if (CraftType == ECraftType::Workbench)
		{
			CraftDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft"));
			if (!CraftDataTable) { UE_LOG(LogTemp, Error, TEXT("UCraftComponent::CachingDT: Failed to load workbench craft DT")); return; }
		}
	}
	// create cache
	for (const TPair<FName, uint8*>& Row : CraftDataTable->GetRowMap())
	{
		const FCraftData* CraftData = reinterpret_cast<const FCraftData*>(Row.Value);
		if (CraftData && CraftData->ItemToCraft) CraftDataCache.Add(CraftData->ItemToCraft, *CraftData);
	}
}

void UCraftComponent::ServerCraftItem_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{
	CraftItemInternal(ItemClass);
}

bool UCraftComponent::ServerCraftItem_Validate(TSubclassOf<AMainItemActor> ItemClass)
{
	return false;
}