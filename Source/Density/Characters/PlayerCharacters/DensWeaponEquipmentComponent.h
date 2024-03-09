// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Items/DensAmmoBrick.h"
#include "Weapons/DensWeaponData.h"
#include "Weapons/DensWeaponDefinition.h"
#include "DensWeaponEquipmentComponent.generated.h"


class UDensWeaponInstance;
class ADensCharacterBase;
enum class EWeaponType : uint8;
class ADensWeapon;
class UDensWeaponData;
class UAnimMontage;




UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Kinetic = 0 UMETA(DisplayName = "Kinetic"),
	Energy = 1 UMETA(DisplayName = "Energy"),
	Heavy = 2 UMETA(DisplayName = "Heavy")
};


/*
*	This holds basic weapon attributes for each weapon outside attribute set
*	that get copied to the active attrtibute set later
*/
USTRUCT(Blueprintable)
struct FWeaponBasicAttributes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentMagAmmo = 0; //Ammo in magazine

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentReserves = 0; //Current total reserves

	UPROPERTY(BlueprintReadOnly)
	EWeaponAmmoType AmmoType = EWeaponAmmoType::AMMOTYPE_Primary; //Type of ammo

	UPROPERTY()
	uint8 ReplicationCounter = 0;
};


/**
 *  This component holds information about the current weapons the player has equipped and manages weapon swaps and spawning weapon actor
 */
UCLASS()
class DENSITY_API UDensWeaponEquipmentComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UDensWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitializeComponent() override;

	UFUNCTION()
	void OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	//For when a new weapon is equipped on character from inventory
	void InitNewWeapon(TSubclassOf<UDensWeaponInstance> WeaponInstanceClass, bool bAlreadyEquipped = false);

	
	void UpdateWeaponInstances(float DeltaTime);
	
	
	UFUNCTION(BlueprintCallable, Category = "Density|DensWeapons|Equip/Unequip")
	void EquipKineticWeapon();

	UFUNCTION(BlueprintCallable, Category = "Density|DensWeapons|Equip/Unequip")
	void EquipEnergyWeapon();

	UFUNCTION(BlueprintCallable, Category = "Density|DensWeapons|Equip/Unequip")
	void EquipHeavyWeapon();

	
	UFUNCTION(Server, Reliable)
	void OnAmmoBrickPickedUp(EWeaponAmmoType AmmoType, float Value);



	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetActiveWeaponMeshFirstPerson() const;

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetActiveWeaponMeshThirdPerson() const;

	
	UFUNCTION(BlueprintPure)
	ADensWeapon* GetActiveWeaponFirstPerson() const;

	UFUNCTION(BlueprintPure)
	ADensWeapon* GetActiveWeaponThirdPerson() const;

	
	UFUNCTION(BlueprintPure)
	UDensWeaponInstance* GetActiveWeaponInstance() const;

	
	//Destroys weapon actors. TODO make it so that weapons ragdoll on floor
	UFUNCTION()
	void OnPlayerDeath(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo);

	UFUNCTION(Client, Reliable)
	void OnPlayerDeath_Client();

	UFUNCTION()
	void OnFirstToThirdPersonMeshSwap(bool bIsFirstPerson);


	UFUNCTION()
	bool IsAmmoTypeFull(EWeaponAmmoType AmmoType);

protected:

	UPROPERTY()
	TWeakObjectPtr<class UDensAbilitySystemComponent> OwnerASC;

	UPROPERTY()
	TWeakObjectPtr<class UDensBaseWeaponStatsAttributeSet> ActiveWeapon_StatsAttributeSet;
	

	void ChangeActiveWeapon(UDensWeaponInstance* NewActiveWeapon);

	// Equips/Unequips the current active weapon
	void EquipWeapon();

	void UnEquipWeapon(); //@TODO: When 2 players, after  player swaps weapons once, the other player will not have their abilities removed next time they swap weapons, creating a new ability handle each time that never gets used. Subsequent ability removals remove and add the enw one, but the old stuck ability is used

	void SpawnWeaponActor(TSubclassOf<ADensWeapon>* WeaponToSpawn);
	void SpawnWeaponActorThirdPerson(TSubclassOf<ADensWeapon>* WeaponToSpawn);
	
	void DestroyWeaponActors();

	
	//Weapon instances
	void DestroyWeaponInstanceObject(EWeaponSlot WeaponSlot);
	
	TObjectPtr<UDensWeaponInstance> SpawnWeaponInstanceObject(TSubclassOf<UDensWeaponInstance> WeaponInstanceToSpawn);
	
public:

	//Active weapon is a pointer to the OwnerWeaponComponents child class
	
	UPROPERTY()
	UDensWeaponInstance* ActiveWeaponInstance;

	
	//For now set the weapon classes in blueprints But should be done from inventory
	/*
	UPROPERTY(EditDefaultsOnly, Category = "Equipped Weapons") 
	TObjectPtr<UDensWeaponInstance> KineticWeaponInstance;


	UPROPERTY(EditDefaultsOnly, Category = "Equipped Weapons")
	TObjectPtr<UDensWeaponInstance> EnergyWeaponInstance;


	UPROPERTY(EditDefaultsOnly, Category = "Equipped Weapons")
	TObjectPtr<UDensWeaponInstance> HeavyWeaponInstance;
*/
	
	
protected:

	//TArray<FWeaponBasicAttributes>* WeaponSavedAttributes; //MOVE TO PS
	
	//FWeaponBasicAttributes* ActiveWeapon_SavedAttributes;
	
	void InitWeaponSavedAttribute( EWeaponSlot Slot);

	void CopyWeaponStatsToActiveAttributeSet();
	
	void SaveActiveAttributesToWeaponStats();

	
public:

	//Getters for current weapon attributes

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetFireDelay() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetMagAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetMaxMagSize() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetAmmoReserves() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetMaxAmmoReserves() const;


	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetShotRecoilDirection() const;

	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponActionAnimationSet GetReadyAnimations() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponActionAnimationSet  GetStowAnimations() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponActionAnimationSet  GetFireAnimations() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponActionAnimationSet GetReloadAnimations() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponActionAnimationSet GetSlideAnimations() const;

	//TODO change this to get idle animation/idle pose
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	FWeaponAnimationData GetWeaponAnimations() const;

	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	bool IsWeaponInstanceValid() const;

	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	FTransform GetActiveWeaponADSOffset() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon|Animations")
	EWeaponSlot GetActiveWeaponSlot() const; //TODO weapon slot shouldnt be determined by the field on WeaponDefinition.
											// and it should prevent the weapon from being equipped in wrong slot in engine

	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|ActiveWeapon")
	float GetActiveWeaponADSShakeModifier();

	
private:

	//Spawned weapons (Mesh for animations and effects)
	UPROPERTY()
	TObjectPtr<ADensWeapon> SpawnedWeapon; //TODO should be local only as this is used only for first person

	UPROPERTY(replicated)
	TObjectPtr<ADensWeapon> SpawnedWeaponThirdPerson; //TODO should be replicated as this is seen in third person for all

	//Spawned instances(stats recoil etc)
	UPROPERTY()
	TArray<TObjectPtr<UDensWeaponInstance>> WeaponInstances;


	//List of gameplay abilities granted for the current weapon
	UPROPERTY()
	FDensAbilitySet_GrantedHandles GrantedAbilityHandles;

private:

	FWeaponBasicAttributes* GetSavedWeaponAttributes(EWeaponSlot Slot);
	
};
