// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.
// Some things from Lyra
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Tickable.h"
#include "Ability/DensDamageSource.h"

#include "DensWeaponInstance.generated.h"

class UDensWeaponData;
class UDensWeaponReticleWidget;
class UDensWeaponEquipmentComponent;
class UUserWidget;
class UDensAbilitySet;
class ADensCharacter;
class AGameplayAbilityTargetActor;

enum class EWeaponSlot : uint8;
enum class EWeaponAmmoType : uint8;


//TODO This could maybe be just a UObject instead of an AActor
UCLASS(Blueprintable)
class DENSITY_API UDensWeaponInstance : public UObject, public IDensDamageSource
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UDensWeaponInstance(const FObjectInitializer& ObjectInitializer);

	
	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	// Getter for TargetTraceActor. Spawns one it if it doesn't exist yet.
	//UFUNCTION(BlueprintCallable, Category = "Density|Targeting")
	//AGameplayAbilityTargetActor* GetTargetActor();
	
	UFUNCTION(BlueprintCallable)
	UDensWeaponData* GetWeaponData() const
	{
		return WeaponData;
	}

	//Return weapon spread on a range from 0 to 1
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetWeaponSpreadNormalizedValue();
	

	TSubclassOf<UDensWeaponReticleWidget> GetHipfireReticleClass()
	{
		return HipFireReticleClass;
	}

	TSubclassOf<UDensWeaponReticleWidget> GetADSReticleClass()
	{
		return ADSReticleClass;
	}
	
	void SetIsEquipped(bool bEquipped) 
	{
		bIsEquipped = bEquipped;
	}


	void SetCurrentAmmo(float Mag, float Reserves) 
	{
		CurrentReserves = Reserves;
		CurrentAmmo = Mag;
	}
	
	float GetCalculatedSpreadAngle() const
	{
		return CurrentSpreadAngle;
	}
	
	float GetCalculatedSpreadAngleMultiplier() const
	{
		return CurrentSpreadAngleMultiplier;
	}

	

	float GetWeaponCritModifier() const
	{
		return WeaponCritModifier;
	}

	// Called every frame from player character
	void UpdateWeaponOnTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void AddSpread();

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanFireWeaponRPM() const;

	
	UFUNCTION(BlueprintCallable)
	FRotator ApplyRecoilAndSpread();

	
	UFUNCTION(BlueprintCallable)
	void DEBUG_PrintInstanceVariables();


	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetFireDelay() const;

	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetWeaponDamage() const;

	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Stats")
	float GetWeaponStaggerDamage() const;

	
	UFUNCTION(BlueprintCallable, Category = "Density | Weapons | Effects")
	FGameplayTag GetFireGameplayCueTag() const;
	
	virtual class UWorld* GetWorld() const;

	
	virtual float GetDistanceAttenuation(float Distance) const override;

public:

protected:
	
	
	
	//virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;
	
	//Returns how much the Characters Pitch and Yaw should be moved by
	FRotator ApplyRecoil();
	
protected:

	UPROPERTY()
	TObjectPtr<AGameplayAbilityTargetActor> TargetActor;

	UPROPERTY()
	TObjectPtr<ADensCharacter> DensCharacterOwner;

	UPROPERTY()
	TObjectPtr<UDensWeaponEquipmentComponent> WeaponEquipmentComponentOwner;
	
	
	// Data asset containing animation data and montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density | Config")
	TObjectPtr<UDensWeaponData> WeaponData;

	
	// Ability sets to grant to the players's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density | Config")
	TArray<TObjectPtr<UDensAbilitySet>> WeaponAbilitySets;

	//Weapon slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density | Config")
	EWeaponSlot WeaponSlot;

	
	// Curve that maps the heat to the spread angle
	// The X range of this curve typically sets the min/max heat range of the weapon
	// The Y range of this curve is used to define the min and maximum spread angle
	UPROPERTY(EditAnywhere, Category = "Density | Spread | Fire Params")
	FRuntimeFloatCurve HeatToSpreadCurve;


	// Curve that maps the current heat to the amount a single shot will further 'heat up'
	// This is typically a flat curve with a single data point indicating how much heat a shot adds,
	// but can be other shapes to do things like punish overheating by adding progressively more heat.
	UPROPERTY(EditAnywhere, Category="Density | Spread | Fire Params")
	FRuntimeFloatCurve HeatToHeatPerShotCurve;


	// Curve that maps the current heat to the heat cooldown rate per second
	// This is typically a flat curve with a single data point indicating how fast the heat
	// wears off, but can be other shapes to do things like punish overheating by slowing down
	// recovery at high heat.
	UPROPERTY(EditAnywhere, Category="Density | Spread | Fire Params")
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;


	// Time since firing before spread cooldown recovery begins (in seconds)
	UPROPERTY(EditAnywhere, Category="Density | Spread | Fire Params", meta=(ForceUnits=s))
	float SpreadRecoveryCooldownDelay = 0.0f;

	// Multiplier when in an aiming camera mode
	UPROPERTY(EditAnywhere, Category="Density | Spread | Fire Params", meta=(ForceUnits=x))
	float SpreadAngleModifier_Aiming = 1.0f;

	// Multiplier when player is in the air
	UPROPERTY(EditAnywhere, Category="Density | Spread | Fire Params", meta=(ForceUnits=x))
	float SpreadAngleModifier_InAir = 2.0f;



	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Targeting")
	float ADSMovementModifier = 0.6;


	// Curve that maps the distance (in cm) to a multiplier on the base damage from the associated gameplay effect
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditAnywhere, Category = "Density | Weapon Config | Targeting")
	FRuntimeFloatCurve DistanceDamageFalloff;


	//How much the weapon shakes up and down when firing
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FVector2D UpDownTranslationRange;

	//How much the weapon shakes sideways when firing
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FVector2D SidewaysTranslationRange;

	//How much the weapon rotates in pitch when firing
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FVector2D PitchRotationRange;

	//How much the weapon rotates in yaw when firing
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FVector2D YawRotationRange;

	//How much the weapon rotates in roll when firing
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FVector2D RollRotationRange;



	// Maximum value for kickback that a weapon can have
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	float MaxKickback = 10.0f;

	// Weapon kickback when Aiming down sights
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	float ADSKickback = 13.0f;


	// Curve that maps the current kickback to the amount each consecutive shot will further kickback
	// A flat line with constant kickback for very fast rate of fire.
	// For slower rate of fire weapons could have a large spike of kickback on the first shot then negative kickback on the remaining
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FRuntimeFloatCurve KickbackPerShotInSprayCurve;

	// Curve that maps the current kickback to the kickback cooldown rate per second
	// This is typically a flat curve with a single data point indicating how fast the heat
	// wears off, but can be other shapes
	UPROPERTY(EditAnywhere, Category="Density | Recoil | Weapon Kickback/Shake")
	FRuntimeFloatCurve KickbackToCoolDownPerSecondCurve;

	// Curve that maps the current recoil strength to heat amount.
	// Typically a flat curve with a single data point representing the recoil intensity if it is not dependant on heat.
	// If more recoil is needed at higher heat then make custom shape curve
	UPROPERTY(EditAnywhere, Category="Density | Recoil | CameraRecoil")
	FRuntimeFloatCurve RecoilToHeatCurve;

	// Time since firing before Recoil recovery begins (in seconds). 
	// Set to -1 to set the Recovery delay to the weapons fire rate
	UPROPERTY(EditAnywhere, Category="Density | Recoil | CameraRecoil")
	float RecoilRecoveryStartDelay = -1.0f;



	// HipFire reticle widget class
	UPROPERTY(EditAnywhere, Category = "Density | Weapon Config | ReticleHUD")
	TSubclassOf<UDensWeaponReticleWidget> HipFireReticleClass;

	// HipFire reticle widget class
	UPROPERTY(EditAnywhere, Category = "Density | Weapon Config | ReticleHUD")
	TSubclassOf<UDensWeaponReticleWidget> ADSReticleClass;

	//UI Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | UI")
	TObjectPtr<UTexture2D> UIIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Ammo")
	int32 MagazineSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Ammo")
	int32 TotalReserves;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Ammo")
	EWeaponAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Ammo")
	float AmmoBrickToAmmoRatio = 1.0f;
	
	
	//Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float WeaponDamage = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float WeaponStaggerDamage = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Density | Weapon Config | Stats", Meta = (Categories = "DamageType"))
	FGameplayTagContainer DamageTypeTags;
	
	//Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 Stability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 Handling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 ReloadSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 RoundsPerMinute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	int32 VerticalRecoilDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Density | GameFeel")
	float WeaponADSShakeModifier = 1.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Density | GameFeel", Meta = (Categories = "GameplayCue"))
	FGameplayTag WeaponFireGameplayCue;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Density | Weapon Config | Stats")
	float WeaponCritModifier = 1.0f;


	

private:
	
	// The current heat
	float CurrentHeat = 0.0f;

	// The current spread angle (in degrees, diametrical)
	float CurrentSpreadAngle = 0.0f;
	
	// The current *combined* spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;

	//Time since weapon was last stopped fired
	double TimeLastFired = 0.0;

	// Number of continous shoots at max fire rate
	int SprayShotCount = 0;

	//Amount of kickback recoil for weapon animation
	float CumulativeKickback = 0.0f;

	//Is character ADS;
	bool bIsADS;

	//Is this instance of the weapon equipped currently
	bool bIsEquipped = false;


	//Used to set initial values on HUD
	UPROPERTY(BlueprintReadOnly, Category = "Density | WeaponAmmo", meta = (AllowPrivateAccess = "true"))
	float CurrentAmmo;
	UPROPERTY(BlueprintReadOnly, Category = "Density | WeaponAmmo", meta = (AllowPrivateAccess = "true"))
	float CurrentReserves;

	
private:

	// Updates the spread and returns true if the spread is at minimum
	bool UpdateSpread(float DeltaSeconds);

	// Updates the multipliers and returns true if they are at minimum
	bool UpdateMultipliers(float DeltaSeconds);

	//Update the kickback transform
	void UpdateKickback(float DeltaSeconds);


	
	void TryResetCamRecoil();
	
	// Resets the kickback value used for the weapon recoil transform
	void TryResetShotCount();
	
	void ComputeSpreadRange(float& MinSpread, float& MaxSpread);
	void ComputeHeatRange(float& MinHeat, float& MaxHeat);
	//void ComputeKickbackRange(float& MinKickback, float& MaxKickback);

	float ComputeRecoilAngleDirection(float RecoilDirectionStat);
	
	FTransform CalculateRecoilShakeTransform();

	FTransform CalculateKickbackTransform();

	
	inline float ClampHeat(float NewHeat)
	{
		float MinHeat;
		float MaxHeat;
		ComputeHeatRange(/*out*/ MinHeat, /*out*/ MaxHeat);

		return FMath::Clamp(NewHeat, MinHeat, MaxHeat);
	}


	inline float ClampKickback(float NewKickback)
	{
		return FMath::Clamp(NewKickback, 0, MaxKickback);
	}
	
	friend class UDensWeaponEquipmentComponent;
};
