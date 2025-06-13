// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterialTagDatabase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PhysicalMaterialTagSubsystem.generated.h"

struct FGameplayTagContainer;
/**
 * 
 */
UCLASS()
class SURVIVALPROJECT_API UPhysicalMaterialTagSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Config")
	UPhysicalMaterialTagDatabase* Database;

	const FGameplayTagContainer* GetTags(const UPhysicalMaterial* Material) const
	{
		return Database ? Database->GetTagsForMaterial(Material) : nullptr;
	}
};
