// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainItemActor.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryStruct.generated.h"

class UInventoryComponent;

/*
 * 
 */
USTRUCT(BlueprintType)
struct FItemInventorySlot : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FItemInventorySlot()
		: Quantity(0)
		, MaxQuantity(0)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMainItemActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxQuantity;

	int32 GetFreeSpace() const { return MaxQuantity - Quantity; }

	FORCEINLINE bool operator==(const FItemInventorySlot& Other) const
	{
		return ItemClass == Other.ItemClass;
	}
};

USTRUCT()
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FItemInventorySlot> Items;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryComponent> Owner;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FItemInventorySlot, FInventoryList>(Items, DeltaParams, *this);
	}

	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};