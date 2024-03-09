// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DensStaggerDamageMMC.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensStaggerDamageMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	
	UDensStaggerDamageMMC();

	
	//UFUNCTION(BlueprintNativeEvent, Category="Calculation")
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;


protected:
	FGameplayEffectAttributeCaptureDefinition IncomingStaggerDamageDef;
	
	FGameplayEffectAttributeCaptureDefinition CritDamageModifierDef;
	
};
