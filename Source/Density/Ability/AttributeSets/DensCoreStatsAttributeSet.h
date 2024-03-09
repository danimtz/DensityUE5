// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "DensCoreStatsAttributeSet.generated.h"


// Macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



UCLASS()
class DENSITY_API UDensCoreStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	UDensCoreStatsAttributeSet();


	//AttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Mobility)
	FGameplayAttributeData Mobility;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Mobility);
	

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Resilience)
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Resilience);


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Recovery)
	FGameplayAttributeData Recovery;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Recovery);


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Discipline)
	FGameplayAttributeData Discipline;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Discipline);


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Intellect)
	FGameplayAttributeData Intellect;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Intellect);


	UPROPERTY(BlueprintReadOnly, Category = "Attributes|CoreStats", ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UDensCoreStatsAttributeSet, Strength);

	


protected:

	UFUNCTION()
	virtual void OnRep_Mobility(const FGameplayAttributeData& OldMobility);

	UFUNCTION()
	virtual void OnRep_Resilience(const FGameplayAttributeData& OldResilience);

	UFUNCTION()
	virtual void OnRep_Recovery(const FGameplayAttributeData& OldRecovery);

	UFUNCTION()
	virtual void OnRep_Discipline(const FGameplayAttributeData& OldDiscipline);

	UFUNCTION()
	virtual void OnRep_Intellect(const FGameplayAttributeData& OldIntellect);

	UFUNCTION()
	virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);


};
