// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DensBaseStatsAttributeSet.generated.h"



// Macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_FourParams(FDensAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);
DECLARE_MULTICAST_DELEGATE(FDensAttributeEventNoParams);


UCLASS()
class DENSITY_API UDensBaseStatsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UDensBaseStatsAttributeSet();


	//AttributeSet overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_WalkSpeedMod)
	FGameplayAttributeData WalkSpeedMod;
	ATTRIBUTE_ACCESSORS(UDensBaseStatsAttributeSet, WalkSpeedMod);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|BaseStats", ReplicatedUsing = OnRep_SprintSpeedMod)
	FGameplayAttributeData SprintSpeedMod;
	ATTRIBUTE_ACCESSORS(UDensBaseStatsAttributeSet, SprintSpeedMod);

	
protected:
	
	
	UFUNCTION()
	virtual void OnRep_WalkSpeedMod(const FGameplayAttributeData& OldWalkSpeedMod);

	UFUNCTION()
	virtual void OnRep_SprintSpeedMod(const FGameplayAttributeData& OldSprintSpeedMod);

	
};
