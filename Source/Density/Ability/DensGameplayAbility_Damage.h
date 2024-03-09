// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensAbilityTypes.h"
#include "DensGameplayAbility.h"
#include "DensGameplayAbility_Damage.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensGameplayAbility_Damage : public UDensGameplayAbility
{
	GENERATED_BODY()

public:
	
	UDensGameplayAbility_Damage(const FObjectInitializer& ObjectInitializer);


	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void ApplyDamageContainerFromHitResults(const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	const FGameplayEffectContextHandle& ApplyDamageContainer(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void SetAbilityDamage(float AbilityDamage);

	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void SetAbilityStaggerDamage(float AbilityStaggerDamage);
	
	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void SetDamageSource(UObject* InDamageSource);
	
	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void SetAbilityDamageTypeTags(FGameplayTagContainer InDamageTypeTags);

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	TSubclassOf<UGameplayEffect> AbilityDamageEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	bool bAbilityCanCrit;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	TSubclassOf<UGameplayEffect> DamageApplicationEffect;

	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage", Meta = (Categories = "DamageType"))
	FGameplayTagContainer DamageTypeTags;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	float AbilityDamage = 0.0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	float AbilityStaggerDamage = 0.0f;
	
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage")
	UObject* DamageSource;
	

protected:
	FDensDamageContainerSpec DamageContainerSpec;

};
