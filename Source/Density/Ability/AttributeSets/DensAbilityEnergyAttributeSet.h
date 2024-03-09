// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DensAbilityEnergyAttributeSet.generated.h"



// Macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



/**
 * 
 */
UCLASS()
class DENSITY_API UDensAbilityEnergyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UDensAbilityEnergyAttributeSet();


	//AttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityEnergy", ReplicatedUsing = OnRep_GrenadeEnergy)
	FGameplayAttributeData GrenadeEnergy;
	ATTRIBUTE_ACCESSORS(UDensAbilityEnergyAttributeSet, GrenadeEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityEnergy", ReplicatedUsing = OnRep_MaxGrenadeEnergy)
	FGameplayAttributeData MaxGrenadeEnergy;
	ATTRIBUTE_ACCESSORS(UDensAbilityEnergyAttributeSet, MaxGrenadeEnergy);


	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityEnergy", ReplicatedUsing = OnRep_MeleeEnergy)
	FGameplayAttributeData MeleeEnergy;
	ATTRIBUTE_ACCESSORS(UDensAbilityEnergyAttributeSet, MeleeEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityEnergy", ReplicatedUsing = OnRep_ClassEnergy)
	FGameplayAttributeData ClassEnergy;
	ATTRIBUTE_ACCESSORS(UDensAbilityEnergyAttributeSet, ClassEnergy);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|AbilityEnergy", ReplicatedUsing = OnRep_SuperEnergy)
	FGameplayAttributeData SuperEnergy;
	ATTRIBUTE_ACCESSORS(UDensAbilityEnergyAttributeSet, SuperEnergy);


protected:

	UFUNCTION()
	virtual void OnRep_GrenadeEnergy(const FGameplayAttributeData& OldGrenadeEnergy);
	
	UFUNCTION()
	virtual void OnRep_MaxGrenadeEnergy(const FGameplayAttributeData& OldMaxGrenadeEnergy);

	UFUNCTION()
	virtual void OnRep_MeleeEnergy(const FGameplayAttributeData& OldMeleeEnergy);

	UFUNCTION()
	virtual void OnRep_ClassEnergy(const FGameplayAttributeData& OldClassEnergy);

	UFUNCTION()
	virtual void OnRep_SuperEnergy(const FGameplayAttributeData& OldSuperEnergy);

};
