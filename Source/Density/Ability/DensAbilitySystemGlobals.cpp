// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Ability/DensAbilitySystemGlobals.h"

#include "Ability/DensGameplayAbilityActorInfo.h"

#include "Ability/DensAbilityTypes.h"

FGameplayAbilityActorInfo* UDensAbilitySystemGlobals::AllocAbilityActorInfo() const
{
	return new FDensGameplayAbilityActorInfo();
}

FGameplayEffectContext* UDensAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FDensGameplayEffectContext();
}
