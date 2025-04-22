// Fill out your copyright notice in the Description page of Project Settings.


#include "Craft/CraftComponent.h"
#include "Engine/Engine.h"
#include "Inventory/MainItemActor.h"
#include "Inventory/InventoryComponent.h"
#include "Craft/CraftData.h"
#include "Engine/DataTable.h"


UCraftComponent::UCraftComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	CraftType = ECraftType::Hand;
}

void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();
	CacheCraftDT();
}

void UCraftComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


FCraftData UCraftComponent::GetCraftItemData(const TSubclassOf<AMainItemActor>& ItemClass)
{
	if (!ItemClass) return FCraftData();
	if (FCraftData* CraftData = CraftDataCache.Find(ItemClass)) return *CraftData;
	UE_LOG(LogTemp, Warning, TEXT("CraftComponent::GetCraftItemData: No craft item data found for class %s"), *ItemClass->GetName());
	return FCraftData();
}

void UCraftComponent::CacheCraftDT()
{
	if (!CraftDataTable)
	{	// load DataTable depending on ECraftType
		if (CraftType == ECraftType::Hand)
		{
			CraftDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft"));
			if (!CraftDataTable)
			{
				UE_LOG(LogTemp, Error, TEXT("UCraftComponent::CachingDT: Failed to load hand craft DT"));
				return;
			}
		}
		if (CraftType == ECraftType::Workbench)
		{
			CraftDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HandCraft.DT_HandCraft"));
			if (!CraftDataTable)
			{
				UE_LOG(LogTemp, Error, TEXT("UCraftComponent::CachingDT: Failed to load workbench craft DT"));
				return;
			}
		}
	}
	// create cache
	for (const TPair<FName, uint8*>& Row : CraftDataTable->GetRowMap())
	{
		FCraftData* CraftData = reinterpret_cast<FCraftData*>(Row.Value);
		if (CraftData && CraftData->ItemToCraft) CraftDataCache.Add(CraftData->ItemToCraft, *CraftData);
	}
}

bool UCraftComponent::bCanCraft(const TSubclassOf<AMainItemActor>& ItemClass)
{
	if (!ItemClass)
	{
		UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Validate: ItemClass is nullptr"));
		return false;
	}
	
	FCraftData CraftData = GetCraftItemData(ItemClass);
	
	if (!CraftData.ItemToCraft)
	{
		UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Validate: ItemCraftData is nullptr"));
		return false;
	}
	
	if (InventoryComponent->FindSlotByClass(ItemClass)->GetFreeSpace() < CraftData.Quantity)
	{
		UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Validate: Player has no enough space"));
		return false;
	}
	
	for (const auto& CraftMaterial : CraftData.CraftMaterials)
	{
		FItemInventorySlot* Slot = InventoryComponent->FindSlotByClass(CraftMaterial.ItemClass);  
		if (Slot->Quantity < CraftMaterial.Quantity)
		{ 
			UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Player has no enough craft materials of type: %s"),
			       *CraftMaterial.ItemClass->GetName()); 
			return false; 
		} 
	}
	
	return true;
}

void UCraftComponent::CraftItem(const TSubclassOf<AMainItemActor>& ItemClass)
{
	ServerCraftItem(ItemClass);
}

void UCraftComponent::ServerCraftItem_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{
	if (!bCanCraft(ItemClass)) return;
	FCraftData CraftData = GetCraftItemData(ItemClass);
	for (auto& Material : CraftData.CraftMaterials)
	{
		InventoryComponent->RemoveItemByClass(Material.ItemClass, Material.Quantity);
	}
	InventoryComponent->AddItemByClass(ItemClass, CraftData.Quantity);
	UE_LOG(LogTemp, Error, TEXT("UCraftComponent: Item was successfully crafted")); 
}

bool UCraftComponent::ServerCraftItem_Validate(TSubclassOf<AMainItemActor> ItemClass)
{
	return bCanCraft(ItemClass);
}