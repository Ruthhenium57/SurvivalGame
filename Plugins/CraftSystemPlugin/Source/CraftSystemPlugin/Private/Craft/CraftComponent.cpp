// Fill out your copyright notice in the Description page of Project Settings.


#include "Craft/CraftComponent.h"
#include "Engine/Engine.h"
#include "Inventory/InventoryComponent.h"
#include "CraftDataSubsystem.h"
#include "CraftLogMacros.h"
#include "CraftSystemPlugin.h"
#include "Craft/CraftData.h"
#include "Engine/DataTable.h"


UCraftComponent::UCraftComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	CraftType = ECraftType::Hand;
}

void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	CraftDataSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCraftDataSubsystem>();
}

void UCraftComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UCraftComponent::CanCraft(int32 ItemID)
{
	if (ItemID < 1)
	{
		LOGF_CRAFT(Error, "ItemID is nullptr")
		return false;
	}
	
	FCraftData* CraftData = CraftDataSubsystem->GetCraftItemData(ItemID, CraftType);
	
	if (!CraftData)
	{
		LOGF_CRAFT(Error, "ItemCraftData is nullptr")
		return false;
	}
	
	if (InventoryComponent->CanAddItems(ItemID, CraftData->Quantity))
	{
		LOGF_CRAFT(Error, "Player has no enough space")
		return false;
	}
	
	for (const auto& CraftMaterial : CraftData->CraftMaterials)
	{
		FItemInventorySlot* Slot = InventoryComponent->FindSlotByID(CraftMaterial.ItemMaterialID);  
		if (Slot->Quantity < CraftMaterial.Quantity)
		{ 
			LOGF_CRAFT(Error, "Player has no enough craft materials of ID: %d", CraftMaterial.ItemMaterialID); 
			return false; 
		} 
	}
	
	return true;
}

void UCraftComponent::CraftItem(int32 ItemID)
{
	ServerCraftItem(ItemID);
}

void UCraftComponent::ServerCraftItem_Implementation(int32 ItemID)
{
	FCraftData* CraftData = CraftDataSubsystem->GetCraftItemData(ItemID, CraftType);
	for (auto& Material : CraftData->CraftMaterials)
	{
		InventoryComponent->RemoveItemByID(Material.ItemMaterialID, Material.Quantity);
	}
	InventoryComponent->AddItemByID(ItemID, CraftData->Quantity);
	LOGF_CRAFT(Display, "Item was successfully crafted");
}

bool UCraftComponent::ServerCraftItem_Validate(int32 ItemID)
{
	return CanCraft(ItemID);
}