// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensCombatComponent.h"

#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"


// Sets default values for this component's properties
UDensCombatComponent::UDensCombatComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	DensASC = nullptr;
	CombatAttributeSet = nullptr;
}

void UDensCombatComponent::OnAbilitySystemComponentInitialized(UDensAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (!InASC)
	{
		UE_LOG(LogTemp, Error, TEXT("DensCombatComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	DensASC = InASC;
	if (!InASC)
	{
		UE_LOG(LogTemp, Error, TEXT("DensCombatComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	CombatAttributeSet = DensASC->GetSet<UDensCombatAttributeSet>();
	if (!CombatAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("DensCombatComponent: Cannot initialize health component for owner [%s] with NULL combat set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	
	CombatAttributeSet->OnDamageTaken.AddUObject(this, &ThisClass::HandleDamageTaken);
	CombatAttributeSet->OnOutOfShield.AddUObject(this, &ThisClass::HandleOutOfShield);
	CombatAttributeSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	CombatAttributeSet->OnStaggerThresholdCrossed.AddUObject(this, &ThisClass::HandleStaggerThresholdCrossed);
	
	
}

float UDensCombatComponent::GetMaxHealth() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetMaxHealth() : 0.0f);
}



float UDensCombatComponent::GetHealth() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetHealth() : 0.0f);
}


float UDensCombatComponent::GetMaxShield() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetMaxShield() : 0.0f);
}


float UDensCombatComponent::GetShield() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetShield() : 0.0f);
}


float UDensCombatComponent::GetCritDamageModifier() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetCritDamageModifier() : 0.0f);
}

float UDensCombatComponent::GetHealthPercentage() const
{
	float CurrHealth = GetHealth();
	float MaxHealth = GetMaxHealth();
	if(MaxHealth != 0.0f)
	{
		return CurrHealth/MaxHealth;
	}
	return 0.0f;

}



float UDensCombatComponent::GetStaggerThreshold() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetStaggerThreshold() : 0.0f);
}



float UDensCombatComponent::GetCurrentStagger() const
{
	return (CombatAttributeSet ? CombatAttributeSet->GetCurrentStagger() : 0.0f);
}



float UDensCombatComponent::GetOutgoingDamage()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetOutgoingDamage() : 0.0f);
}

void UDensCombatComponent::SetOutgoingDamage(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetOutgoingDamage(NewValue);
}

float UDensCombatComponent::GetOutgoingStaggerDamage()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetOutgoingStaggerDamage() : 0.0f);
}

void UDensCombatComponent::SetOutgoingStaggerDamage(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetOutgoingStaggerDamage(NewValue);
}

float UDensCombatComponent::GetOutgoingEmpowerBuff()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetOutgoingEmpowerBuff() : 0.0f);
}

void UDensCombatComponent::SetOutgoingEmpowerBuff(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetOutgoingEmpowerBuff(NewValue);
}

float UDensCombatComponent::GetOutgoingSurgeBuff()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetOutgoingSurgeBuff() : 0.0f);
}

void UDensCombatComponent::SetOutgoingSurgeBuff(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetOutgoingSurgeBuff(NewValue);
}

float UDensCombatComponent::GetIncomingDamage()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetIncomingDamage() : 0.0f);
}

void UDensCombatComponent::SetIncomingDamage(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetIncomingDamage(NewValue);
}

float UDensCombatComponent::GetIncomingStaggerDamage()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetIncomingStaggerDamage() : 0.0f);
}

void UDensCombatComponent::SetIncomingStaggerDamage(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetIncomingStaggerDamage(NewValue);
}


float UDensCombatComponent::GetIncomingWeakenDebuff()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetIncomingWeakenDebuff() : 0.0f);
}

void UDensCombatComponent::SetIncomingWeakenDebuff(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetIncomingWeakenDebuff(NewValue);
}

float UDensCombatComponent::GetIncomingDamageResistBuff()
{
	return (CombatAttributeSet ? CombatAttributeSet->GetIncomingDamageResistBuff() : 0.0f);
}

void UDensCombatComponent::SetIncomingDamageResistBuff(float NewValue)
{
	const_cast<UDensCombatAttributeSet*>(CombatAttributeSet.Get())->SetIncomingDamageResistBuff(NewValue);
}


void UDensCombatComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
                                             const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnOutOfHealth.Broadcast(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}


void UDensCombatComponent::HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnOutOfShield.Broadcast(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}


void UDensCombatComponent::HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnDamageTaken.Broadcast(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}

void UDensCombatComponent::HandleStaggerThresholdCrossed(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnStaggerThresholdCrossed.Broadcast(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);
}



