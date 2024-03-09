// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "DensWeaponDefinition.h"
#include "Ability/DensAbilitySet.h"

#include "DensWeaponData.generated.h"

class ADensWeapon;
class USkeletalMesh;
class UAnimBlueprint;
class UAnimMontage;
class UAnimSequence;




USTRUCT(BlueprintType)
struct FWeaponActionAnimationSet
{
	GENERATED_BODY()

public:

	FWeaponActionAnimationSet() : WeaponMontage(nullptr),  FirstPersonMontage(nullptr), ThirdPersonMontage(nullptr)
	{
	}
	
	FWeaponActionAnimationSet(UAnimMontage* InWeaponMontage, UAnimMontage* InFirstPersonMontage, UAnimMontage* InThirdPersonMontage )
	: WeaponMontage(InWeaponMontage), FirstPersonMontage(InFirstPersonMontage),  ThirdPersonMontage(InThirdPersonMontage)
	{
	}

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> WeaponMontage; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage>  FirstPersonMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage>  ThirdPersonMontage;

	
};



USTRUCT(BlueprintType)
struct DENSITY_API FWeaponAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations Poses")
	TObjectPtr<UAnimSequence> IdlePose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations Poses")
	TObjectPtr<UAnimSequence> NonCombatPose;

};


/**
 * 
 */
UCLASS()
class DENSITY_API UDensWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:

	UDensWeaponData(const FObjectInitializer& ObjectInitializer);



	
public:


	
	//TODO weapon perks should be abilities?

	//Weapon Stats
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Weapon Stat Definition")
	//TObjectPtr<UDensWeaponDefinition> WeaponDefinition;

	//Weapon Blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Weapon To Spawn")
	TSubclassOf<ADensWeapon> WeaponToSpawn;

	//Weapon Animations Montages  TODO Move to seperate Data Asset class WeaponAnimations?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponActionAnimationSet StowAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponActionAnimationSet EquipAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponActionAnimationSet FireAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponActionAnimationSet ReloadAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponActionAnimationSet SlideAnimation;


	//TODO change this to actionanimation sets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Animations")
	FWeaponAnimationData AnimationPoses;
	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Weapon Mesh")
	TObjectPtr<USkeletalMesh> WeaponSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Weapon AnimBP")
	TObjectPtr<UAnimBlueprint> WeaponAnimBP;
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Density|ADSOffset")
	FTransform WeaponADSOffset;

	
};