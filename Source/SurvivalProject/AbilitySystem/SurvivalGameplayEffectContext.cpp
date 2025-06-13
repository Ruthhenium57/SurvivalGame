// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGameplayEffectContext.h"

// #if UE_WITH_IRIS
// #include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
// #include "Serialization/GameplayEffectContextNetSerializer.h"
// #endif

#include "SurvivalAbilitySourceInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalGameplayEffectContext)

class FArchive;

FSurvivalGameplayEffectContext* FSurvivalGameplayEffectContext::ExtractEffectContext(
	struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if (BaseEffectContext && BaseEffectContext->GetScriptStruct()->IsChildOf(
		FSurvivalGameplayEffectContext::StaticStruct()))
	{
		return static_cast<FSurvivalGameplayEffectContext*>(BaseEffectContext);
	}

	return nullptr;
}

void FSurvivalGameplayEffectContext::SetAbilitySource(const ISurvivalAbilitySourceInterface* InObject,
                                                      float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const ISurvivalAbilitySourceInterface* FSurvivalGameplayEffectContext::GetAbilitySource() const
{
	return Cast<ISurvivalAbilitySourceInterface>(AbilitySourceObject.Get());
}

bool FSurvivalGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

const UPhysicalMaterial* FSurvivalGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}
//
// #if UE_WITH_IRIS
// namespace UE::Net
// {
// 	// Forward to FGameplayEffectContextNetSerializer
// 	// Note: If FLyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
// 	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(LyraGameplayEffectContext,
// 	                                                                 FGameplayEffectContextNetSerializer);
// }
// #endif
