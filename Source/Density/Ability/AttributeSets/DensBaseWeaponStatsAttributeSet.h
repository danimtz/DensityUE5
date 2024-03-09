// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DensBaseWeaponStatsAttributeSet.generated.h"



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
class DENSITY_API UDensBaseWeaponStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UDensBaseWeaponStatsAttributeSet();


	//AttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, Damage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_MagAmmo)
	FGameplayAttributeData MagAmmo;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, MagAmmo);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_MaxMagSize)
	FGameplayAttributeData MaxMagSize;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, MaxMagSize);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_AmmoReserves)
	FGameplayAttributeData AmmoReserves;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, AmmoReserves);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_MaxAmmoReserves)
	FGameplayAttributeData MaxAmmoReserves;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, MaxAmmoReserves);


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_RoundsPerMinute)
	FGameplayAttributeData RoundsPerMinute;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, RoundsPerMinute);

	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_VerticalRecoil)
	FGameplayAttributeData VerticalRecoil;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, VerticalRecoil);
	

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|WeaponStats", ReplicatedUsing = OnRep_ADSMovementModifier)
	FGameplayAttributeData ADSMovementModifier;
	ATTRIBUTE_ACCESSORS(UDensBaseWeaponStatsAttributeSet, ADSMovementModifier);


	

	//Add things like Range, Recoil direction, Handling, Stability etc etc etc here

protected:

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage);

	
	UFUNCTION()
	virtual void OnRep_MaxMagSize(const FGameplayAttributeData& OldMaxMagSize);


	UFUNCTION()
	virtual void OnRep_MaxAmmoReserves(const FGameplayAttributeData& OldMaxAmmoReserves);



	UFUNCTION()
	virtual void OnRep_MagAmmo(const FGameplayAttributeData& OldMagAmmo);



	UFUNCTION()
	virtual void OnRep_AmmoReserves(const FGameplayAttributeData& OldAmmoReserves);


	UFUNCTION()
	virtual void OnRep_RoundsPerMinute(const FGameplayAttributeData& OldRoundsPerMinute);

	
	UFUNCTION()
	virtual void OnRep_VerticalRecoil(const FGameplayAttributeData& OldVerticalRecoil);

	
	UFUNCTION()
	virtual void OnRep_ADSMovementModifier(const FGameplayAttributeData& OldADSMovementModifier);
};
