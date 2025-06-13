// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SurvivalInputConfig.generated.h"


class UInputAction;

USTRUCT(Blueprintable)
struct FSurvivalInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};
/**
 * 
 */
UCLASS(BlueprintType, Const)
class SURVIVALPROJECT_API USurvivalInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bNotFoundLog = true) const;
	
	UFUNCTION(BlueprintCallable, Category = "Survival|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bNotFoundLog = true) const;

	// list of IA used by owner. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FSurvivalInputAction> NativeInputActions;

	// list of IA used by owner.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FSurvivalInputAction> AbilityInputActions;
};
