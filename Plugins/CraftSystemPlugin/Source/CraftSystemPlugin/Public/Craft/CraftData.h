// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct CRAFTSYSTEMPLUGIN_API FCraftMaterialData : public FTableRowBase
{
	GENERATED_BODY()

	FCraftMaterialData()
		: ItemMaterialID(0)
		, Quantity(1)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	int32 ItemMaterialID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	int32 Quantity;
};

USTRUCT(BlueprintType)
struct CRAFTSYSTEMPLUGIN_API FCraftData : public FTableRowBase
{
	GENERATED_BODY()

	FCraftData()
		: ItemToCraft(0)
		, Quantity(1)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemToCraft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<FCraftMaterialData> CraftMaterials;
};