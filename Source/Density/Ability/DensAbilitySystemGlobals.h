// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "DensAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()



	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;
	
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
