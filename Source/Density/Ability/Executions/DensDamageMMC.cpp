// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensDamageMMC.h"

#include "Ability/DensAbilityTypes.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"


UDensDamageMMC::UDensDamageMMC()
{

	IncomingDamageDef.AttributeToCapture = UDensCombatAttributeSet::GetIncomingDamageAttribute();
	IncomingDamageDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IncomingDamageDef.bSnapshot = false;

	CritDamageModifierDef.AttributeToCapture = UDensCombatAttributeSet::GetCritDamageModifierAttribute();
	CritDamageModifierDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	CritDamageModifierDef.bSnapshot = false;

	IncomingWeakenDebuffDef.AttributeToCapture = UDensCombatAttributeSet::GetIncomingWeakenDebuffAttribute();
	IncomingWeakenDebuffDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IncomingWeakenDebuffDef.bSnapshot = false;

	IncomingDamageResistDef.AttributeToCapture = UDensCombatAttributeSet::GetIncomingDamageResistBuffAttribute();
	IncomingDamageResistDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IncomingDamageResistDef.bSnapshot = false;
	
	RelevantAttributesToCapture.Add(IncomingDamageDef);
	RelevantAttributesToCapture.Add(IncomingDamageResistDef);
	RelevantAttributesToCapture.Add(IncomingWeakenDebuffDef);
	RelevantAttributesToCapture.Add(CritDamageModifierDef);

	
}


float UDensDamageMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	

	FDensGameplayEffectContext* Context = static_cast<FDensGameplayEffectContext*>(Spec.GetContext().Get());
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//Capture incoming damage, weaken debuff and crit damage multiplier
	float IncomingDamage = 0.0f;
	GetCapturedAttributeMagnitude(IncomingDamageDef, Spec, EvaluationParameters, IncomingDamage);

	float WeakenDebuff = 0.0f;
	GetCapturedAttributeMagnitude(IncomingWeakenDebuffDef, Spec, EvaluationParameters, WeakenDebuff);

	float DamageResist = 0.0f;
	GetCapturedAttributeMagnitude(IncomingDamageResistDef, Spec, EvaluationParameters, DamageResist);
	
	
	float CritDamageMod = 0.0f;
	GetCapturedAttributeMagnitude(CritDamageModifierDef, Spec, EvaluationParameters, CritDamageMod);
	
	CritDamageMod = FMath::Max<float>(CritDamageMod, 0.0f);

	
	
	float TotalDamageTaken = IncomingDamage * WeakenDebuff * DamageResist;
	
	if(Context->IsCriticalHit())
	{
		TotalDamageTaken = TotalDamageTaken * CritDamageMod ;
	}
	
	
	return TotalDamageTaken;

}

