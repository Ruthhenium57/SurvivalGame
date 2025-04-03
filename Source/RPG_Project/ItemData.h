// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	AllTypes		UMETA(DisplayName = "AllTypes"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Tool			UMETA(DisplayName = "Tool"),
	Medicine		UMETA(DisplayName = "Medicine"),
	Food			UMETA(DisplayName = "Food"),
	Component		UMETA(DisplayName = "Component"),
	Object			UMETA(DisplayName = "Object")
};


USTRUCT(BlueprintType)
struct RPG_PROJECT_API FItemData: public FTableRowBase
{
	GENERATED_BODY()

	FItemData()
		: MaxQuantity(1)
		, ItemType(EItemType::AllTypes)
		, ItemImage(nullptr)
	{}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<class AMainItemActor> ItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString InteractTextBlockName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString InteractTextBlockName2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UTexture2D> ItemImage;
};

