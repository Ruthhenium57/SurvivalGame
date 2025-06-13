// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IMCDataAsset.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS(BlueprintType)
class SURVIVALPROJECT_API UIMCAndPriority : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsGlobal;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSetting;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	FName ContextId;
};

UCLASS(BlueprintType)
class SURVIVALPROJECT_API UIMCDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TSoftObjectPtr<UIMCAndPriority>> AllIMCSettings;
};
