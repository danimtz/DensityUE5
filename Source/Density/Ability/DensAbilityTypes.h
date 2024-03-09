// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensDamageSource.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

#include "DensAbilityTypes.generated.h"

class IDensDamageSource;
class UDensDamageSource;
class UGameplayEffect;


//ADD GAMEPLAY SOURCE


/**
 *  Struct defining damage calc effect, damage application effect and damage type tags
 *  These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 */
USTRUCT(BlueprintType)
struct DENSITY_API FDensDamageContainer
{
	GENERATED_BODY()
	
public:
	
	FDensDamageContainer() {}

	
	/** Damage calculation gameplay effect, This is only used to calculate the base damage of the ability */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	TSubclassOf<UGameplayEffect> BaseDamageEffectClass;

	/** Damage Application gameplay effect. Damage type tags will be applied to the gameplay effect context of this spec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	TSubclassOf<UGameplayEffect> DamageApplyEffectClass;

	/** Extra gameplay effects. Additional gameplay effects to be applied by the container. These can include things like movement speed debuffs etc */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> ExtraEffectClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageTypeTags, Meta = (Categories = "DamageType"))
	FGameplayTagContainer DamageTypeTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	bool bDamageCanCrit;

	//Should implement IDamageSource to function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	TObjectPtr<UObject> DamageSource;
};



/** A "processed" version of DensDamageContainer that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct DENSITY_API FDensDamageContainerSpec
{
	GENERATED_BODY()

public:
	FDensDamageContainerSpec() {}

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	
	/** Damage calculation gameplay effect spec */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayEffectSpecHandle BaseDamageEffectSpec;

	/** Damage Application gameplay effect spec. Effect context have damage types tags*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayEffectSpecHandle DamageApplyEffectSpec;

	/** Damage Application gameplay effect spec. Effect context have damage types tags*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> ExtraEffectSpecs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	bool bDamageCanCrit = false;
	
	/** Returns true if this has any valid effect specs */
	//bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	//bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults);

	/** Adds new targets to target data replacing old ones */
	void ReplaceTargets(const TArray<FHitResult>& HitResults);

	
	TObjectPtr<UObject> DamageSource;

	
	IDensDamageSource* GetDamageSource() const
	{
		return Cast<IDensDamageSource>(DamageSource);
	};
};







/**
 *  Custom gameplay effect context
 */
USTRUCT(BlueprintType)
struct DENSITY_API FDensGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:

	virtual FGameplayAbilityTargetDataHandle GetTargetData()
	{
		return TargetData;
	}

	virtual void AddTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
	{
		TargetData.Append(TargetDataHandle);
	}

	virtual void SetTargetData(const FGameplayAbilityTargetDataHandle& TargetData);

	
	virtual bool IsCriticalHit() const {return bIsCriticalHit;}
	virtual void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

	virtual bool IsWeakened() const {return bIsWeakened;}
	virtual void SetIsWeakened(bool bInIsWeakened) { bIsWeakened = bInIsWeakened; }
	
	
	virtual const FGameplayTagContainer& GetDamageTypeTags() const {return DamageTypesTags;}
	virtual void SetDamageTypeTags(const FGameplayTagContainer& InDamageTypeTags) { DamageTypesTags = InDamageTypeTags; }
    

protected:

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsWeakened = false;
	
	UPROPERTY()
	FGameplayTagContainer DamageTypesTags;

	UPROPERTY()	
	FGameplayAbilityTargetDataHandle TargetData;
	
public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
 
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FDensGameplayEffectContext* Duplicate() const override
	{
		FDensGameplayEffectContext* NewContext = new FDensGameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);

		}
		NewContext->SetDamageTypeTags(DamageTypesTags);

		// Shallow copy of TargetData, is this okay?
		NewContext->TargetData.Append(TargetData);
		
		return NewContext;
	}
 
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
};

template <>
struct TStructOpsTypeTraits<FDensGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FDensGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};