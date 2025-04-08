// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MainItemActor.h"
#include "CraftData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct RPG_PROJECT_API FMaterialData : public FTableRowBase
{
	GENERATED_BODY()

	FMaterialData() = default;
		
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TSubclassOf<class AMainItemActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct RPG_PROJECT_API FCraftData : public FTableRowBase
{
	GENERATED_BODY()

	FCraftData() = default;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class AMainItemActor> ItemToCraft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<FMaterialData> Materials;
};