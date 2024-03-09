// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGameplayAbility_Damage.h"

#include "DensGameplayTags.h"

UDensGameplayAbility_Damage::UDensGameplayAbility_Damage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}



void UDensGameplayAbility_Damage::ApplyDamageContainerFromHitResults(const TArray<FHitResult>& HitResults)
{
	if(AbilityDamageEffect && DamageApplicationEffect)
	{
		FDensDamageContainer DamageContainer;
		DamageContainer.BaseDamageEffectClass = AbilityDamageEffect;
		DamageContainer.DamageApplyEffectClass = DamageApplicationEffect;
		DamageContainer.DamageTypeTags = DamageTypeTags;
		DamageContainer.bDamageCanCrit = bAbilityCanCrit;
		
		DamageContainerSpec = MakeDamageContainerSpecFromContainer(DamageContainer);

		DamageContainerSpec.AddTargets(HitResults);

		ApplyDamageContainerSpec(DamageContainerSpec);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityDamageEffect or DamageApplicationEffect not set in [%s]"),  *GetNameSafe(this));
	}
	
	
	
}

const FGameplayEffectContextHandle& UDensGameplayAbility_Damage::ApplyDamageContainer(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if(AbilityDamageEffect && DamageApplicationEffect)
	{
		FDensDamageContainer DamageContainer;
		DamageContainer.BaseDamageEffectClass = AbilityDamageEffect;
		DamageContainer.DamageApplyEffectClass = DamageApplicationEffect;
		DamageContainer.DamageTypeTags = DamageTypeTags;
		DamageContainer.bDamageCanCrit = bAbilityCanCrit;
		
		DamageContainerSpec = MakeDamageContainerSpecFromContainer(DamageContainer);

		DamageContainerSpec.TargetData = TargetData;

		DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingDamage, AbilityDamage);
		DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingStaggerDamage, AbilityStaggerDamage);
		DamageContainerSpec.DamageSource = DamageSource;

		
		ApplyDamageContainerSpec(DamageContainerSpec);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AbilityDamageEffect or DamageApplicationEffect not set in [%s]"),  *GetNameSafe(this));
	}

	//Add target data to the effect context for use in gameplay cues
	FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(DamageContainerSpec.DamageApplyEffectSpec.Data->GetContext().Get());
	if (EffectContext)
	{
		EffectContext->AddTargetData(TargetData);
	}
	
	return DamageContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext();
}


void UDensGameplayAbility_Damage::SetAbilityDamage(float InAbilityDamage)
{
	AbilityDamage = InAbilityDamage;
	
}

void UDensGameplayAbility_Damage::SetAbilityStaggerDamage(float InAbilityStaggerDamage)
{
	AbilityStaggerDamage = InAbilityStaggerDamage;
}


void UDensGameplayAbility_Damage::SetDamageSource(UObject* InDamageSource)
{
	DamageSource = InDamageSource;
}



void UDensGameplayAbility_Damage::SetAbilityDamageTypeTags(FGameplayTagContainer InDamageTypeTags)
{
	DamageTypeTags = InDamageTypeTags;
	
}
