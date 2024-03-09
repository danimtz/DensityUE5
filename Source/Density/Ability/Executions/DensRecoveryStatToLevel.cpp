// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensRecoveryStatToLevel.h"

#include "Ability/AttributeSets/DensCoreStatsAttributeSet.h"

UDensRecoveryStatToLevel::UDensRecoveryStatToLevel()
{

	RecoveryDef.AttributeToCapture = UDensCoreStatsAttributeSet::GetRecoveryAttribute();
	RecoveryDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RecoveryDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(RecoveryDef);
}

float UDensRecoveryStatToLevel::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();


	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	

	float Recovery = 0.f;
	GetCapturedAttributeMagnitude(RecoveryDef, Spec, EvaluationParameters, Recovery);
	Recovery = FMath::Max<float>(Recovery, 0.0f);

	
	float RecoveryLevel = floor(Recovery * 0.1) + 1;
	
	return RecoveryLevel;
}




