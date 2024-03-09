// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensDamageExecCalculation.h"

#include "Ability/DensAbilityTypes.h"
#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"


// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct DensDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingStaggerDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamageModifier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaggerDamageTaken);
	DensDamageStatics()
	{
		
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCombatAttributeSet, IncomingDamage, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCombatAttributeSet, IncomingStaggerDamage, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCombatAttributeSet, CritDamageModifier, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCombatAttributeSet, DamageTaken, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCombatAttributeSet, StaggerDamageTaken, Target, false);
		
		// TODO Capture resilience for damage resists maybe????
		//DEFINE_ATTRIBUTE_CAPTUREDEF(UDensCoreStatsAttributeSet, Resilience???, Target, false);
	}
};

static const DensDamageStatics& DamageStatics()
{
	static DensDamageStatics DStatics;
	return DStatics;
}



UDensDamageExecCalculation::UDensDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().IncomingStaggerDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageModifierDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageTakenDef);
	RelevantAttributesToCapture.Add(DamageStatics().StaggerDamageTakenDef);
}

void UDensDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//BOILERPLATE Add tags and parameters to Evaluation parameters
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FDensGameplayEffectContext* Context = static_cast<FDensGameplayEffectContext*>(Spec.GetContext().Get());
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	

	
	//Capture incoming damage and crit damage multiplier
	float IncomingDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().IncomingDamageDef, EvaluationParameters, IncomingDamage);

	float CritDamageMod = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageModifierDef, EvaluationParameters, CritDamageMod);
	CritDamageMod = FMath::Max<float>(CritDamageMod, 0.0f);


	float IncomingStaggerDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().IncomingStaggerDamageDef, EvaluationParameters, IncomingStaggerDamage);

	//TODO add stagger damamge resistance here?

	
	float TotalDamageTaken = IncomingDamage;
	
	if(Context->IsCriticalHit())
	{
		TotalDamageTaken = IncomingDamage * CritDamageMod;
	}


	float TotalStaggerDamageTaken = IncomingStaggerDamage;
	
	// Set the Target's damage taken meta attribute
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageTakenProperty, EGameplayModOp::Override, TotalDamageTaken));

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().StaggerDamageTakenProperty, EGameplayModOp::Override, TotalStaggerDamageTaken));
	


	
}
