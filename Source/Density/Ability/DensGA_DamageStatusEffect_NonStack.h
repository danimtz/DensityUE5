// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensGameplayAbility.h"
#include "DensGA_DamageStatusEffect_NonStack.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensGA_DamageStatusEffect_NonStack : public UDensGameplayAbility
{
	GENERATED_BODY()

	UDensGA_DamageStatusEffect_NonStack(const FObjectInitializer& ObjectInitializer);
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StatusEffectPercent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "DamageType"))
	FGameplayTagContainer DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "Gameplay.StatusEffect"))
	FGameplayTag DamageStatusEffectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

};
