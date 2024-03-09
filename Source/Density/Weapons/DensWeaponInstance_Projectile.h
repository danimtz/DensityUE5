// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensWeaponInstance.h"
#include "Ability/DensDamageSource.h"
#include "DensWeaponInstance_Projectile.generated.h"

class ADensWeaponProjectile;
class ADensGATargetActor_WeaponProjectile;
/**
 * 
 */
UCLASS()
class DENSITY_API UDensWeaponInstance_Projectile : public UDensWeaponInstance
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetProjectileBlastDamage() const;


	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetProjectileBlastStaggerDamage() const;


	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Density | Weapon Config | Stats", Meta = (Categories = "DamageType"))
	FGameplayTagContainer BlastDamageTypeTags;
	
	UFUNCTION(BlueprintCallable, Category = "Density|Targeting")
	ADensGATargetActor_WeaponProjectile* GetTraceTargetActor();
	

	float GetProjectileBlastRadius()
	{
		return ProjectileBlastRadius;
	}

	float GetProjectileVelocity()
	{
		return ProjectileVelocity;
	}
	
	
	FRuntimeFloatCurve& GetBlastDistanceDamageFalloff()
	{
		return BlastDistanceDamageFalloff;
	}


	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetProjectileDirection();


	
	
protected:


	
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Density | Weapon Config | Projectile")
	TSubclassOf<ADensWeaponProjectile> ProjectileClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float ProjectileVelocity = 10000.0f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float ProjectileBlastStaggerDamage = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float ProjectileBlastDamage = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float ProjectileBlastRadius = 0.0f;

	
	UPROPERTY(EditAnywhere, Category = "Density | Weapon Config | Projectile")
	FRuntimeFloatCurve BlastDistanceDamageFalloff;
	

};
 