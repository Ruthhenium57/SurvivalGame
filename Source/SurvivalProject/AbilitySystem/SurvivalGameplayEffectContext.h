// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "SurvivalGameplayEffectContext.generated.h"

class ISurvivalAbilitySourceInterface;
class AActor;
class FArchive;
class UObject;
class UPhysicalMaterial;

/**
 * 
 */
USTRUCT()
struct FSurvivalGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	FSurvivalGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FSurvivalGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FSurvivalGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static SURVIVALPROJECT_API FSurvivalGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilitySource(const ISurvivalAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const ISurvivalAbilitySourceInterface* GetAbilitySource() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FSurvivalGameplayEffectContext* NewContext = new FSurvivalGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSurvivalGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;

	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID = -1;

protected:
	/** Ability Source object (should implement ISurvivalAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template<>
struct TStructOpsTypeTraits<FSurvivalGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FSurvivalGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};