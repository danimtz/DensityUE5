// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DensAbilityLibrary.generated.h"

class UDensGameplayAbility;
class ADensCharacterBase;
struct FDensDamageContainerSpec;
struct FGameplayTagContainer;
/**
 * 
 */
UCLASS()
class DENSITY_API UDensAbilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "DensAbilityLibrary | Effects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContext);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static void SetIsCriticalHit( UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bIsCriticalHit);


	UFUNCTION(BlueprintPure, Category = "DensAbilityLibrary | Effects")
	static bool IsWeakened(const FGameplayEffectContextHandle& EffectContext);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static void SetIsWeakened( UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bIsWeakened);


	// Returns TargetData
	UFUNCTION(BlueprintCallable, Category = "DensAbilityLibrary|Effects", Meta = (DisplayName = "GetTargetData"))
	static FGameplayAbilityTargetDataHandle GetTargetData(FGameplayEffectContextHandle EffectContext);

	// Adds TargetData
	UFUNCTION(BlueprintCallable, Category = "DensAbilityLibrary|Effects", Meta = (DisplayName = "AddTargetDataToEffectContext"))
	static void AddTargetDataToEffectContext(FGameplayEffectContextHandle EffectContext, const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable, Category = "DensAbilityLibrary|Effects", Meta = (DisplayName = "AddTargetDataToEffectContext"))
	static void SetTargetDataInEffectContext(FGameplayEffectContextHandle EffectContext, const FGameplayAbilityTargetDataHandle& TargetData);

	// CreateTarget data
	UFUNCTION(BlueprintCallable, Category = "DensAbilityLibrary|Effects", Meta = (DisplayName = "CreateTargetDataFromHitResults"))
	static void CreateTargetDataFromHitResults(const TArray<FHitResult>& HitResults, FGameplayAbilityTargetDataHandle& TargetData);

	
	
	UFUNCTION(BlueprintPure , Category = "DensAbilityLibrary | Effects")
	static FGameplayTagContainer GetDamageTypeTags( const FGameplayEffectContextHandle& EffectContext);

	//TODO: allow damage types to be changed in before damage events. basically add a function here to change damage type tags

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& AddTargetsToDamageContainerSpec(  UPARAM(ref) FDensDamageContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& ReplaceTargetsToDamageContainerSpec(  UPARAM(ref) FDensDamageContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults);


	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& SetAbilityDamageInDamageContainer(UPARAM(ref) FDensDamageContainerSpec& DamageContainerSpec, float AbilityDamage);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& SetAbilityStaggerDamageInDamageContainer(UPARAM(ref) FDensDamageContainerSpec& DamageContainerSpec, float AbilityStaggerDamage);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& SetAbilityDamageAndStaggerInDamageContainer(UPARAM(ref) FDensDamageContainerSpec& DamageContainerSpec, float AbilityDamage, float AbilityStaggerDamage);

	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static FDensDamageContainerSpec& SetDamageSourceInDamageContainer(UPARAM(ref) FDensDamageContainerSpec& DamageContainerSpec, UObject* DamageSource);

	
	
	UFUNCTION(BlueprintCallable , Category = "DensAbilityLibrary | Effects")
	static void ApplyExternalDamageContainer( AActor* Instigator, const FDensDamageContainerSpec& DamageContainerSpec);


	
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UDensGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UDensGameplayAbility* GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static bool IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	static bool IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle);



	
	//Enemy Check
	UFUNCTION(BlueprintCallable, Category = "DensAbilityLibrary|Faction", Meta = (DisplayName = "CheckIsEnemy"))
	static bool CheckIsEnemy(const AActor* SelfActor, const AActor* OtherActor);



	//Disable Bone Collisions
	UFUNCTION(BlueprintCallable, Category = "DensLibrary|SkeletalMesh", Meta = (DisplayName = "ChangeBoneCollision"))
	static void ChangeCollisionOnPhysicsBody(USkeletalMeshComponent* SkeletalMesh, FName BoneName, ECollisionEnabled::Type CollisionType, bool bAffectChildren = false);
	
	
};
