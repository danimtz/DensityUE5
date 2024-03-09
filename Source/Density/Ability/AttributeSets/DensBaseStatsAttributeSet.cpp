// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"

#include "DensGameplayTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Ability/DensAbilityTypes.h"
#include "Characters/DensCharacterBase.h"

#include "Net/UnrealNetwork.h"
#include "Player/DensPlayerController.h"


//Initialize attributes.
UDensBaseStatsAttributeSet::UDensBaseStatsAttributeSet() : WalkSpeedMod(1.0f), SprintSpeedMod(1.0f)//WalkSpeed(600.0f), SprintSpeed(1000.0f)
{

}

void UDensBaseStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

}

void UDensBaseStatsAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
}

void UDensBaseStatsAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	
}

void UDensBaseStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
}

void UDensBaseStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseStatsAttributeSet, WalkSpeedMod, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseStatsAttributeSet, SprintSpeedMod, COND_None, REPNOTIFY_Always);
}


void UDensBaseStatsAttributeSet::OnRep_WalkSpeedMod(const FGameplayAttributeData& OldWalkSpeedMod)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseStatsAttributeSet, WalkSpeedMod, OldWalkSpeedMod);
}

void UDensBaseStatsAttributeSet::OnRep_SprintSpeedMod(const FGameplayAttributeData& OldSprintSpeedMod)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseStatsAttributeSet, SprintSpeedMod, OldSprintSpeedMod);
}

