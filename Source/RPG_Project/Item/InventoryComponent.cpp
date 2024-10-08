// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "InvenroryWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ItemsSlots);
}


void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(AMainItemActor* Item)
{
	if (GetOwner()->HasAuthority())
	{
		bool bSuccess = AddItemInternal(Item);
		if (bSuccess)
		{
			FItemInventorySlot OutSlot;
			bool bOutSlot = FindSlotByClass(Item->GetClass(), OutSlot);
			if (bOutSlot)
			{
				MulticastUpdateSlotWidget(OutSlot);
			}
		}
		return bSuccess;
	}
	else
	{
		ServerAddItem(Item);
		return false;
	}
}

bool UInventoryComponent::RemoveItem(AMainItemActor* Item)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		bool bSuccess = RemoveItemInternal(Item);
		if (bSuccess)
		{
			FItemInventorySlot OutSlot;
			bool bOutSlot = FindSlotByClass(Item->GetClass(), OutSlot);
			if (bOutSlot)
			{
				MulticastUpdateSlotWidget(OutSlot);
			}
		}
		return bSuccess;
	}
	else
	{
		ServerRemoveItem(Item);
		return false;
	}
}

void UInventoryComponent::LogInventory() const
{
	
}

bool UInventoryComponent::FindSlotByClass(TSubclassOf<AMainItemActor> ItemClass, FItemInventorySlot& OutItemSlot)
{
	for (FItemInventorySlot& ItemSlot : ItemsSlots)
	{
		if (ItemSlot.ItemClass == ItemClass)
		{
			OutItemSlot = ItemSlot;
			return true;
		}
	}

	static FItemInventorySlot EmptySlot;
	EmptySlot.ItemClass = ItemClass;
	OutItemSlot = EmptySlot;
	return false;
}

void UInventoryComponent::OnRep_Inventory()
{
	
}

void UInventoryComponent::ServerAddItem_Implementation(AMainItemActor* Item)
{
	AddItem(Item);
}

bool UInventoryComponent::ServerAddItem_Validate(AMainItemActor* Item)
{
	return true;
}

void UInventoryComponent::ServerRemoveItem_Implementation(AMainItemActor* Item)
{
	RemoveItemInternal(Item);
}

bool UInventoryComponent::ServerRemoveItem_Validate(AMainItemActor* Item)
{
	return true;
}

bool UInventoryComponent::AddItemInternal(AMainItemActor* Item)
{
	if (Item)
	{
		FItemData* ItemData = nullptr;
		UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Item.DT_Item"));
		if (ItemDataTable)
		{
			FName RowName = FName(Item->GetClass()->GetName().RightChop(7).LeftChop(2));
			ItemData = ItemDataTable->FindRow<FItemData>(RowName, TEXT(""));
		}

		if (ItemData)
		{
			if (!ItemsSlots.IsEmpty())
			{
				for (FItemInventorySlot& Slot : ItemsSlots)
				{
					if (Slot.ItemClass == Item->GetClass())
					{
						if (Slot.Items.Num() < ItemData->MaxQuantity)
						{
							Slot.Items.Add(Item);
							UE_LOG(LogTemp, Display, TEXT("ItemAddedToOldSlot"));
							return true;
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Not enough space"));
							return false;
						}
					}
				}
			}

			FItemInventorySlot Slot;
			Slot.ItemClass = Item->GetClass();
			Slot.Items.Add(Item);
			ItemsSlots.Add(Slot);
			UE_LOG(LogTemp, Display, TEXT("New slot is created"));
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveItemInternal(AMainItemActor* Item)
{
	if (Item)
	{
		if (!ItemsSlots.IsEmpty())
		{
			for (FItemInventorySlot& Slot : ItemsSlots)
			{
				if (Slot.ItemClass == Item->GetClass())
				{
					if (Slot.Items.Num() == 1)
					{
						Slot.Items.Remove(Item);
						if(PlayerWidget)
						{
							PlayerWidget->InventoryWidget->RemoveSlot(Slot.ItemClass);
						}
						ItemsSlots.Remove(Slot);
						UE_LOG(LogTemp, Display, TEXT("Slot is removed"));
						return true;
					}
					else
					{
						if (PlayerWidget)
						{
							PlayerWidget->InventoryWidget->UpdateSlotInfo(Slot);
						}
						Slot.Items.Remove(Item);
						UE_LOG(LogTemp, Display, TEXT("Item removed from slot"));
						return true;
					}
				}
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Item not been removed"));
	return false;
}

void UInventoryComponent::MulticastUpdateSlotWidget_Implementation(FItemInventorySlot ItemSlot)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (Pawn->IsLocallyControlled())
		{
			if (PlayerWidget)
			{
				PlayerWidget->InventoryWidget->UpdateSlotInfo(ItemSlot);
				UE_LOG(LogTemp, Warning, TEXT("Inventory updated on local client"));
			}
		}
	}
}

void UInventoryComponent::MulticastRemoveSlotWidget_Implementation(TSubclassOf<AMainItemActor> ItemClass)
{

}
