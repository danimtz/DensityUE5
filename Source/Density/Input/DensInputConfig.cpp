// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensInputConfig.h"

#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "InputAction.h"

UDensInputConfig::UDensInputConfig(const FObjectInitializer& ObjectInitializer)
{
}


const UInputAction* UDensInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDensInputActionTagPair& ActionPair : NativeInputActionTagPairs)
	{
		if (ActionPair.InputAction && (ActionPair.InputTag == InputTag))
		{
			return ActionPair.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputActionTagPair for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UDensInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FDensInputActionTagPair& ActionPair : AbilityInputActionTagPairs)
	{
		if (ActionPair.InputAction && (ActionPair.InputTag == InputTag))
		{
			return ActionPair.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputActionTagPair for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}


	return nullptr;
}