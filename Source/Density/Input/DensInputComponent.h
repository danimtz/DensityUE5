// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DensInputConfig.h"
#include "InputAction.h"
#include "GameplayTagContainer.h"

#include "DensInputComponent.generated.h"


class UInputAction;

/**
 * 
 */
UCLASS()
class DENSITY_API UDensInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	


public:

	// Bind native action to an input tag
	template<class UserClass, typename FuncType>
	void BindNativeActionByTag(const UDensInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);


	// Bind ability to an input tag
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UDensInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);


	//From Lyra TODO add later maybe
	//void RemoveBinds(TArray<uint32>& BindHandles);

	//Can add functions for custom input mappings here
};

template<class UserClass, typename FuncType>
void UDensInputComponent::BindNativeActionByTag(const UDensInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
};


template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UDensInputComponent::BindAbilityActions(const UDensInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{


	check(InputConfig);
	for (const FDensInputActionTagPair& Action : InputConfig->AbilityInputActionTagPairs)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}