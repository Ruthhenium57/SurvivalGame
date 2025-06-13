// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftComponent.generated.h"

class UCraftDataSubsystem;
class UInventoryComponent;

UENUM(BlueprintType)
enum class ECraftType : uint8
{
	Hand			UMETA(DisplayName = "Hand"),
	Workbench		UMETA(DisplayName = "Workbench"),
	MAX UMETA(Hidden)
};

ENUM_RANGE_BY_FIRST_AND_LAST(ECraftType, ECraftType::Hand, ECraftType::Workbench);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAFTSYSTEMPLUGIN_API UCraftComponent : public UActorComponent
{
	GENERATED_BODY()

	UCraftComponent();

protected:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UFUNCTION(BlueprintCallable, Category = "Craft")
	bool CanCraft(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Craft")
	void CraftItem(int32 ItemID);
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft", meta = (AllowPrivateAccess))
	ECraftType CraftType;

	UPROPERTY()
	TObjectPtr<UCraftDataSubsystem> CraftDataSubsystem;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCraftItem(int32 ItemID);
};
