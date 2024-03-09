// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DensWeaponDefinition.generated.h"




UENUM(BlueprintType)
enum class EWeaponType : uint8{
	WEAPONTYPE_Kinetic	UMETA(DisplayName = "Kinetic"), 
	WEAPONTYPE_Energy	UMETA(DisplayName = "Energy"), 
	WEAPONTYPE_Heavy	UMETA(DisplayName = "Heavy")
};

UENUM(BlueprintType)
enum class EWeaponAmmoType : uint8 {
	AMMOTYPE_Primary	UMETA(DisplayName = "Primary"),
	AMMOTYPE_Special	UMETA(DisplayName = "Special"),
	AMMOTYPE_Power		UMETA(DisplayName = "Power")
};


UCLASS(BlueprintType, Const)
class DENSITY_API UDensWeaponDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:

	UDensWeaponDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

//Weapon slot
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Slot")
	EWeaponType WeaponType;

//Frame/Damage

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float WeaponCritModifier;


	
//Ammo

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MagazineSize;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 TotalReserves;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo Type")
	EWeaponAmmoType AmmoType;

//Stats
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 Range;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 Stability;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 Handling;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 ReloadSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 RoundsPerMinute;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 VerticalRecoil;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ADSMovementModifier;


	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetFireDelayFromRPM() const;

	//TODO create a weapon archetype class?

	//TODO add hidden stats

protected:

};
