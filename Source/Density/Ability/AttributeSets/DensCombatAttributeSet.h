// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DensCombatAttributeSet.generated.h"



// Macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_FourParams(FDensAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);
DECLARE_MULTICAST_DELEGATE(FDensAttributeEventNoParams);


UCLASS()
class DENSITY_API UDensCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UDensCombatAttributeSet();


	//AttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ClampAttributeOnChanged(const FGameplayAttribute& Attribute, float& NewValue) const;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, Shield);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_MaxShield)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, MaxShield);
	

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_CritDamageModifier)
	FGameplayAttributeData CritDamageModifier;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, CritDamageModifier);
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_StaggerThreshold)
	FGameplayAttributeData StaggerThreshold;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, StaggerThreshold);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_CurrentStagger)
	FGameplayAttributeData CurrentStagger;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, CurrentStagger);
	
	
	//Meta attributes

	//Outgoing damage buckets
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	mutable FGameplayAttributeData OutgoingDamage;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, OutgoingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData OutgoingEmpowerBuff;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, OutgoingEmpowerBuff);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData OutgoingSurgeBuff;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, OutgoingSurgeBuff);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	mutable FGameplayAttributeData OutgoingStaggerDamage;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, OutgoingStaggerDamage);

	
	//Incoming damage buckets
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData IncomingWeakenDebuff;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, IncomingWeakenDebuff);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData IncomingDamageResistBuff;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, IncomingDamageResistBuff);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData IncomingStaggerDamage;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, IncomingStaggerDamage);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, DamageTaken);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats")
	FGameplayAttributeData StaggerDamageTaken;
	ATTRIBUTE_ACCESSORS(UDensCombatAttributeSet, StaggerDamageTaken);
	

	mutable FDensAttributeEvent OnOutOfHealth;
	mutable FDensAttributeEvent OnOutOfShield;
	mutable FDensAttributeEvent OnDamageTaken;
	mutable FDensAttributeEventNoParams OnFullHealthBar; 
	mutable FDensAttributeEvent OnStaggerThresholdCrossed; 

	
	
protected:
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	
	UFUNCTION()
	virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	virtual void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

	
	UFUNCTION()
	virtual void OnRep_CritDamageModifier(const FGameplayAttributeData& OldCritDamageModifier);


	UFUNCTION()
	virtual void OnRep_StaggerThreshold(const FGameplayAttributeData& OldStaggerThreshold);
	
	UFUNCTION()
	virtual void OnRep_CurrentStagger(const FGameplayAttributeData& OldCurrentStagger);
	

	bool bOutOfHealth;
	bool bOutOfShield;
	bool bIsFullHealthBar;
	bool bStaggerThresholdCrossed;
};
