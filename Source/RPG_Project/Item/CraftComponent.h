// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.h"
#include "CraftComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PROJECT_API UCraftComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Craft")
	bool CraftItemInternal(TSubclassOf<AMainItemActor> ItemClass);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Craft")
	UDataTable* CraftDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;
};
