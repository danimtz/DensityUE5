// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGA_DamageStatusEffect_NonStack.h"

UDensGA_DamageStatusEffect_NonStack::UDensGA_DamageStatusEffect_NonStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}
