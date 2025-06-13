// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SurvivalInputComponent)

class USurvivalInputConfig;

USurvivalInputComponent::USurvivalInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void USurvivalInputComponent::AddInputMappings(const USurvivalInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void USurvivalInputComponent::RemoveInputMappings(const USurvivalInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void USurvivalInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (const uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
