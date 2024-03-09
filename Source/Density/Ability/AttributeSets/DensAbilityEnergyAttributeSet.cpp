// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAbilityEnergyAttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#include "Net/UnrealNetwork.h"


UDensAbilityEnergyAttributeSet::UDensAbilityEnergyAttributeSet()
: GrenadeEnergy(100.0f), MaxGrenadeEnergy(100.0f), MeleeEnergy(100.0f),  ClassEnergy(100.0f), SuperEnergy(100.0f)
{

	
}


void UDensAbilityEnergyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);


	
}

void UDensAbilityEnergyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	if (Data.EvaluatedData.Attribute == GetGrenadeEnergyAttribute())
	{
		// Clamp GrenadeEnergy
		SetGrenadeEnergy(FMath::Clamp(GetGrenadeEnergy(), 0.0f, GetMaxGrenadeEnergy()));
	}
}

void UDensAbilityEnergyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDensAbilityEnergyAttributeSet, GrenadeEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensAbilityEnergyAttributeSet, MaxGrenadeEnergy, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDensAbilityEnergyAttributeSet, MeleeEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensAbilityEnergyAttributeSet, ClassEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensAbilityEnergyAttributeSet, SuperEnergy, COND_None, REPNOTIFY_Always);
}

void UDensAbilityEnergyAttributeSet::OnRep_GrenadeEnergy(const FGameplayAttributeData& OldGrenadeEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensAbilityEnergyAttributeSet, GrenadeEnergy, OldGrenadeEnergy);
}

void UDensAbilityEnergyAttributeSet::OnRep_MaxGrenadeEnergy(const FGameplayAttributeData& OldMaxGrenadeEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensAbilityEnergyAttributeSet, MaxGrenadeEnergy, OldMaxGrenadeEnergy);
}


void UDensAbilityEnergyAttributeSet::OnRep_MeleeEnergy(const FGameplayAttributeData& OldMeleeEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensAbilityEnergyAttributeSet, MeleeEnergy, OldMeleeEnergy);
}

void UDensAbilityEnergyAttributeSet::OnRep_ClassEnergy(const FGameplayAttributeData& OldClassEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensAbilityEnergyAttributeSet, ClassEnergy, OldClassEnergy);
}

void UDensAbilityEnergyAttributeSet::OnRep_SuperEnergy(const FGameplayAttributeData& OldSuperEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensAbilityEnergyAttributeSet, SuperEnergy, OldSuperEnergy);
}
