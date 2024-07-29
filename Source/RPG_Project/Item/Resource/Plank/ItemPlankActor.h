// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ResourceItemActor.h"
#include "ItemPlank.h"
#include "ItemPlankActor.generated.h"

/**
 * 
 */
UCLASS()
class RPG_PROJECT_API AItemPlankActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;
};