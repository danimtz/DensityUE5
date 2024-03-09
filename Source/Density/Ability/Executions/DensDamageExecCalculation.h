// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DensDamageExecCalculation.generated.h"

/**
 * CURRENTLY UNUSED
 */
UCLASS()
class DENSITY_API UDensDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UDensDamageExecCalculation();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
