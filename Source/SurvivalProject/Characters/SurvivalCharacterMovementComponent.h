// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SurvivalCharacterMovementComponent.generated.h"

SURVIVALPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 *	FSurvivalCharacterGroundInfo
 *
 *	Information about the ground under the character. It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FSurvivalCharacterGroundInfo
{
	GENERATED_BODY()

	FSurvivalCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/**
 *	USurvivalCharacterMovementComponent
 *
 *	The base character movement component
 */
UCLASS(Config = Game)
class SURVIVALPROJECT_API USurvivalCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USurvivalCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "Suvival|CharacterMovement")
	const FSurvivalCharacterGroundInfo& GetGroundInfo();

	//~	Begin of UMovementComponent interface
	virtual float GetMaxSpeed() const override;
	//~ End of UMovementComponent interface

protected:

	virtual void InitializeComponent() override;

	// Cached ground info for the character.  Do not access this directly!
	FSurvivalCharacterGroundInfo CachedGroundInfo;
};
