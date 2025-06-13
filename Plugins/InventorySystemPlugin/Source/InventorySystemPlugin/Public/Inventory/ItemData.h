// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Inventory/MainItemActor.h"
#include "ItemData.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Invalid = 0		UMETA(Hidden),

	AllTypes		UMETA(DisplayName = "AllTypes"),
	Weapon			UMETA(DisplayName = "Weapon"),
	Tool			UMETA(DisplayName = "Tool"),
	Medicine		UMETA(DisplayName = "Medicine"),
	Food			UMETA(DisplayName = "Food"),
	Component		UMETA(DisplayName = "Component"),
	Object			UMETA(DisplayName = "Object"),
	Max				UMETA(Hidden)
};


USTRUCT(BlueprintType)
struct INVENTORYSYSTEMPLUGIN_API FItemData: public FTableRowBase
{
	GENERATED_BODY()

	FItemData()
		: ItemID(0)
		, MaxQuantity(1)
		, ItemType(EItemType::AllTypes)
		, ItemImage(nullptr)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TOptional<EItemType> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AMainItemActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
};

