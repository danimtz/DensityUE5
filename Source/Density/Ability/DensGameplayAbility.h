// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DensGameplayAbility.generated.h"


struct FDensDamageContainer;
class UDensAbilitySystemComponent;
struct FDensDamageContainerSpec;

UENUM(BlueprintType)
enum class EDensAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	OnSpawn
};


USTRUCT()
struct DENSITY_API FAbilityMeshMontage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class USkeletalMeshComponent* Mesh;

	UPROPERTY()
	class UAnimMontage* Montage;

	FAbilityMeshMontage() : Mesh(nullptr), Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* InMesh, class UAnimMontage* InMontage) 
		: Mesh(InMesh), Montage(InMontage)
	{
	}
};




/*
 *	Used to pass weapon animation data to be used in gameplay abilities
 */
USTRUCT(BlueprintType)
struct FWeaponActionMeshSet
{
	GENERATED_BODY()

public:

	FWeaponActionMeshSet() :  WeaponMeshFirstPerson(nullptr),  FirstPersonMesh(nullptr), ThirdPersonMesh(nullptr)
	{
	}
	
	FWeaponActionMeshSet(USkeletalMeshComponent* InWeaponMeshFP, USkeletalMeshComponent* InWeaponMeshTP, USkeletalMeshComponent* InFirstPersonMesh, USkeletalMeshComponent* InThirdPersonMesh  )
	:  WeaponMeshFirstPerson(InWeaponMeshFP), WeaponMeshThirdPerson(InWeaponMeshTP),FirstPersonMesh(InFirstPersonMesh), ThirdPersonMesh(InThirdPersonMesh)
	{
	}
	
	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMeshFirstPerson; 

	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMeshThirdPerson; 
	
	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* FirstPersonMesh;
	
	UPROPERTY(BlueprintReadOnly)
	USkeletalMeshComponent* ThirdPersonMesh;
	
};




/**
 * 
 */
UCLASS()
class DENSITY_API UDensGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UDensGameplayAbility(const FObjectInitializer& ObjectInitializer);
	

	EDensAbilityActivationPolicy GetActivationPolicy() const {return ActivationPolicy;};
	

	UFUNCTION(BlueprintPure, Category = DamageContainers)
	FDensDamageContainerSpec MakeDamageContainerSpecFromContainer(const FDensDamageContainer& Container);

	UFUNCTION(BlueprintCallable, Category = DamageContainers)
	void ApplyDamageContainerSpec(const FDensDamageContainerSpec& Container);


	UFUNCTION(BlueprintCallable, Category = Ability)
	UDensAbilitySystemComponent* GetDensAbilitySystemComponentFromActorInfo() const;

	// To use with batching system to end the ability externally.
	virtual void ExternalEndAbility();


	// Attempts to activate the given ability handle and batch all RPCs into one. This will only batch all RPCs that happen
	// in one frame. Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);



	
	UFUNCTION(BlueprintCallable, Category = "WeaponAction")
	FWeaponActionMeshSet GetAnimationMeshes() const;
	
	

	// ----------------------------------------------------------------------------------------------------------------
	//	Animation Support for multiple USkeletalMeshComponents on the AvatarActor
	// ----------------------------------------------------------------------------------------------------------------

	/** Returns the currently playing montage for this ability, if any */
	UFUNCTION(BlueprintCallable, Category = Animation)
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/** Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events */
	virtual void SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, class UAnimMontage* InCurrentMontage);

	
protected:

	friend class UDensAbilitySystemComponent;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Density | Ability Activation")
	EDensAbilityActivationPolicy ActivationPolicy;


	// ----------------------------------------------------------------------------------------------------------------
	//	Animation Support for multiple USkeletalMeshComponents on the AvatarActor
	// ----------------------------------------------------------------------------------------------------------------

	/** Active montages being played by this ability */
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;

	bool FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage);

	/** Immediately jumps the active montage to a section */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
	void MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName);

	/** Sets pending section on active montage */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
	void MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	/**
	 * Stops the current animation montage.
	 *
	 * @param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	/**
	* Stops all currently animating montages
	*
	* @param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForAllMeshes(float OverrideBlendOutTime = -1.0f);
};


