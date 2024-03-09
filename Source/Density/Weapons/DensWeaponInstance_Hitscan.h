// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensWeaponInstance.h"
#include "Ability/DensDamageSource.h"
#include "DensWeaponInstance_Hitscan.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensWeaponInstance_Hitscan : public UDensWeaponInstance
{
	GENERATED_BODY()

public:

	
	float GetBulletsPerShot() const
	{
		return BulletsPerShot;
	}

	float GetTraceSweepRadius() const
	{
		return SweepRadius;
	}
	
	float GetMaxDamageRange() const
	{
		return MaxDamageRange;
	}



	UFUNCTION(BlueprintCallable, Category = "Density|Targeting")
	ADensGATargetActor_WeaponTrace* GetTraceTargetActor();



	
	
	
protected:
	
	// Number of bullets to fire in a single shot (typically 1, but may be more for shotguns) (Unused for projectile weapons)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Density | Weapon Config | Targeting")
	int32 BulletsPerShot = 1;

	// Radius of sweep. 0 Means line trace for weapon (Unused for projectile weapons)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Density | Weapon Config | Targeting")
	float SweepRadius = 0.0f;

	
	// The maximum distance at which this weapon can deal damage (Unused for projectile weapons)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Density | Weapon Config | Targeting", meta=(ForceUnits=cm))
	float MaxDamageRange = 25000.0f;

	
	//Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 Range;


	
};
