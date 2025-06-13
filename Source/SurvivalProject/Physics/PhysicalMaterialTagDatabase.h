// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicalMaterialTagDatabase.generated.h"


USTRUCT(BlueprintType)
struct FPhysicalMaterialTagEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	UPhysicalMaterial* Material;

	// Tags for material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	FGameplayTagContainer Tags;
};

/**
 * USurvivalSurfaceTagAsset
 *
 * Data Asset that has mapping of FPhysicalSurface and FGameplayTagContainer
 */
UCLASS(BlueprintType)
class SURVIVALPROJECT_API UPhysicalMaterialTagDatabase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Materials")
	TArray<FPhysicalMaterialTagEntry> Entries;

	/** Find tags for material */
	const FGameplayTagContainer* GetTagsForMaterial(const UPhysicalMaterial* Material) const
	{
		for (auto& E : Entries)
		{
			if (E.Material == Material)
				return &E.Tags;
		}
		return nullptr;
	}
};
