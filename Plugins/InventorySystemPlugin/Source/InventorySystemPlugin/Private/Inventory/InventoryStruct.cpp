// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryStruct.h"
#include "Inventory/InventoryComponent.h"

void FInventoryList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		UE_LOG(LogTemp, Warning, TEXT("Add"));

		FItemInventorySlot LocalItem = Items[Index];
		TWeakObjectPtr<UInventoryComponent> WeakOwner = Owner;
		
		AsyncTask(ENamedThreads::GameThread, [LocalItem, WeakOwner]()
		{
			if (WeakOwner.IsValid())
			{
				WeakOwner->OnInventorySlotAdded.Broadcast(LocalItem, false);
			}
		});
	}
}

void FInventoryList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		UE_LOG(LogTemp, Warning, TEXT("Change"));
		
		FItemInventorySlot LocalItem = Items[Index];
		TWeakObjectPtr<UInventoryComponent> WeakOwner = Owner;
		
		AsyncTask(ENamedThreads::GameThread, [LocalItem, WeakOwner]()
		{
			if (WeakOwner.IsValid())
			{
				WeakOwner->OnInventorySlotChanged.Broadcast(LocalItem, false);
			}
		});
	}
}

void FInventoryList::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove"));
		
		FItemInventorySlot LocalItem = Items[Index];
		TWeakObjectPtr<UInventoryComponent> WeakOwner = Owner;
		
		AsyncTask(ENamedThreads::GameThread, [LocalItem, WeakOwner]()
		{
			if (WeakOwner.IsValid())
			{
				WeakOwner->OnInventorySlotRemoved.Broadcast(LocalItem, false);
			}
		});
	}
}