// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalInputConfig.h"

#include "InventoryLogMacros.h"

const UInputAction* USurvivalInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,
                                                                      bool bNotFoundLog) const
{
	for (const auto& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bNotFoundLog)
	{
		LOGF_FUNC(Warning, "Can't find NativeInputAction for InputTag [%s] on InputConfig [%s].", *InputTag.ToString(),
		          *GetNameSafe(this))
	}

	return nullptr;
}

const UInputAction* USurvivalInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                       bool bNotFoundLog) const
{
	for (const auto& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bNotFoundLog)
	{
		LOGF_FUNC(Warning, "Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s].", *InputTag.ToString(),
				  *GetNameSafe(this))
	}

	return nullptr;
}
