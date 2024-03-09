// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "DensRecoveryStatToLevel.generated.h"

/**
 * Class used to convert Recovery value in 10s to 1s for the curve table.
 * NOT USED CURRENTLY, since exporting CSV curve table can support having values from 0 to 10, 10 to 20 etc.
 * But leaving it here in case I need a MMC class
 */
UCLASS()
class DENSITY_API UDensRecoveryStatToLevel : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	
	UDensRecoveryStatToLevel();

	
	//UFUNCTION(BlueprintNativeEvent, Category="Calculation")
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;


protected:
	FGameplayEffectAttributeCaptureDefinition RecoveryDef;
};
