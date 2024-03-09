// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Ability/AttributeSets/DensCoreStatsAttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"


#include "Net/UnrealNetwork.h"

//Initialize attributes. (For now to 30, change to 0 later)
UDensCoreStatsAttributeSet::UDensCoreStatsAttributeSet() : Mobility(30.0f), Resilience(30.0f), Recovery(30.0f), Discipline(30.0f), Intellect(30.0f), Strength(30.0f)
{
}

void UDensCoreStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDensCoreStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

}

void UDensCoreStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Mobility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Recovery, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Discipline, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Intellect, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCoreStatsAttributeSet, Strength, COND_None, REPNOTIFY_Always);
}


void UDensCoreStatsAttributeSet::OnRep_Mobility(const FGameplayAttributeData& OldMobility)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Mobility, OldMobility);
}

void UDensCoreStatsAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Resilience, OldResilience);
}


void UDensCoreStatsAttributeSet::OnRep_Recovery(const FGameplayAttributeData& OldRecovery)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Recovery, OldRecovery);
}


void UDensCoreStatsAttributeSet::OnRep_Discipline(const FGameplayAttributeData& OldDiscipline)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Discipline, OldDiscipline);
}


void UDensCoreStatsAttributeSet::OnRep_Intellect(const FGameplayAttributeData& OldIntellect)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Intellect, OldIntellect);
}


void UDensCoreStatsAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCoreStatsAttributeSet, Strength, OldStrength);
}
