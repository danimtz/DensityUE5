// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DensDamageMMC.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensDamageMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	
	UDensDamageMMC();

	
	//UFUNCTION(BlueprintNativeEvent, Category="Calculation")
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;


protected:
	FGameplayEffectAttributeCaptureDefinition IncomingDamageDef;

	FGameplayEffectAttributeCaptureDefinition IncomingWeakenDebuffDef;
	
	FGameplayEffectAttributeCaptureDefinition IncomingDamageResistDef;
	
	FGameplayEffectAttributeCaptureDefinition CritDamageModifierDef;
	
};
