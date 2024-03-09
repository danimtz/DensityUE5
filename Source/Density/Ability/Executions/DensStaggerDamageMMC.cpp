// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensStaggerDamageMMC.h"

#include "Ability/DensAbilityTypes.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"

UDensStaggerDamageMMC::UDensStaggerDamageMMC()
{

	IncomingStaggerDamageDef.AttributeToCapture = UDensCombatAttributeSet::GetIncomingStaggerDamageAttribute();
	IncomingStaggerDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IncomingStaggerDamageDef.bSnapshot = false;

	CritDamageModifierDef.AttributeToCapture = UDensCombatAttributeSet::GetCritDamageModifierAttribute();
	CritDamageModifierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	CritDamageModifierDef.bSnapshot = false;

	
	RelevantAttributesToCapture.Add(IncomingStaggerDamageDef);
	RelevantAttributesToCapture.Add(CritDamageModifierDef);

	
}


float UDensStaggerDamageMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	

	FDensGameplayEffectContext* Context = static_cast<FDensGameplayEffectContext*>(Spec.GetContext().Get());
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	
	float IncomingStaggerDamage = 0.0f;
	GetCapturedAttributeMagnitude(IncomingStaggerDamageDef, Spec, EvaluationParameters, IncomingStaggerDamage);
	
	
	float CritDamageMod = 0.0f;
	GetCapturedAttributeMagnitude(CritDamageModifierDef, Spec, EvaluationParameters, CritDamageMod);
	
	CritDamageMod = FMath::Max<float>(CritDamageMod, 0.0f);

	
	
	float TotalStaggerDamageTaken = IncomingStaggerDamage;
	
	if(Context->IsCriticalHit())
	{
		TotalStaggerDamageTaken = TotalStaggerDamageTaken * CritDamageMod ;
	}
	
	
	return TotalStaggerDamageTaken;

}

