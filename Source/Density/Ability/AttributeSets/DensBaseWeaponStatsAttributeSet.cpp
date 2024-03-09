// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Ability/AttributeSets/DensBaseWeaponStatsAttributeSet.h"

#include "Net/UnrealNetwork.h"


//Initialize attributes.
UDensBaseWeaponStatsAttributeSet::UDensBaseWeaponStatsAttributeSet()
{
}

void UDensBaseWeaponStatsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UDensBaseWeaponStatsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

}

void UDensBaseWeaponStatsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, MaxMagSize, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, MaxAmmoReserves, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, MagAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, AmmoReserves, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, VerticalRecoil, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensBaseWeaponStatsAttributeSet, ADSMovementModifier, COND_None, REPNOTIFY_Always);
}


void UDensBaseWeaponStatsAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, Damage, OldDamage);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_MaxMagSize(const FGameplayAttributeData& OldMaxMagSize)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, MaxMagSize, OldMaxMagSize);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_MaxAmmoReserves(const FGameplayAttributeData& OldMaxAmmoReserves)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, MaxAmmoReserves, OldMaxAmmoReserves);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_MagAmmo(const FGameplayAttributeData& OldMagAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, MagAmmo, OldMagAmmo);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_AmmoReserves(const FGameplayAttributeData& OldAmmoReserves)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, AmmoReserves, OldAmmoReserves);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_RoundsPerMinute(const FGameplayAttributeData& OldRoundsPerMinute)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, RoundsPerMinute, OldRoundsPerMinute);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_VerticalRecoil(const FGameplayAttributeData& OldVerticalRecoil)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, VerticalRecoil, OldVerticalRecoil);
}

void UDensBaseWeaponStatsAttributeSet::OnRep_ADSMovementModifier(const FGameplayAttributeData& OldADSMovementModifier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensBaseWeaponStatsAttributeSet, ADSMovementModifier, OldADSMovementModifier);
}
