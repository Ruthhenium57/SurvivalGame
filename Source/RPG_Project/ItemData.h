// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct RPG_PROJECT_API FItemData: public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMainItemActor> ItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InteractTextBlockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InteractTextBlockName2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemImage;
};

