// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Armor/DensArmorInfo.h"
#include "Characters/DensCharacterBase.h"

#include "DensCharacter.generated.h"

class UDensPlayerCharacterData;
class UInputComponent;
class USpringArmComponent;
class UCameraComponent;
class UDensWeaponEquipmentComponent;

struct FInputActionValue;
struct FGameplayTag;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirstThirdPersonSwapDelegate, bool, bIsFirstPerson);


UCLASS()
class DENSITY_API ADensCharacter : public ADensCharacterBase //AModularCharacter?
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	ADensCharacter(const class FObjectInitializer& ObjectInitializer);

	// Implement IAbilitySystemInterface
	//UFUNCTION(BlueprintCallable, Category = "Density | Character")
	//UDensAbilitySystemComponent* GetDensAbilitySystemComponent() const;
	//virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	
	

	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	//~ End AActor Interface



	//~ Begin ACharacter interface, dont call directly. Overrides the base Character functions 
	virtual void OnStartCrouch(float HeightAdjust, float ScaledHeightAdjust) override;
	virtual void OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust) override;




	
	


	//Delegate fired when swapping from third to first person
	UPROPERTY(BlueprintAssignable)
	FOnFirstThirdPersonSwapDelegate OnFirstThirdPersonSwapDelegate; 

	
	
	
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|PerspectiveSwap")
	void PerspectiveSwap(bool bIsFirstPerson);

	bool IsInFirstPerson() { return bIsInFirstPerson; };
	
	UFUNCTION(BlueprintImplementableEvent , Category = "Density|DensCharacter|WeaponAnimation")
	void ApplyAnimationRecoil(const FTransform& Transform);

	
	void SetIsRecoilRecoveryActive(bool bActive) {bIsRecoilRecoveryActive = bActive;};
	
	UDensWeaponEquipmentComponent* GetWeaponEquipmentComponent() const;

	
	//Used to remove HUD from bound playercontroller from locally controlled player
	UFUNCTION()
	void OnPlayerDeath(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo);

	//Returns the location from which to spawn and target line traces for weapons etc. Should be equal to the camera position in first person
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Combat")
	FVector GetTargetingLocation();

	
protected:
	//Override base funcitonality to allow character to jump crouched
	virtual bool CanJumpInternal_Implementation() const override;

	//~ End ACharacter interface,

	
	virtual void HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude) override;

protected:
	

	// Ability system component weak object pointer. This should be obtained from the PlayerState on the possess function
	//UPROPERTY()
	//TWeakObjectPtr<class UDensAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TWeakObjectPtr<class UDensCoreStatsAttributeSet> CoreStatsAttributeSet;

	//UPROPERTY()
	//TWeakObjectPtr<class UDensBaseStatsAttributeSet> BaseStatsAttributeSet;
	
	UPROPERTY()
	TWeakObjectPtr<class UDensAbilityEnergyAttributeSet> AbilityEnergyAttributeSet;
	
	//Used for ability system init. Maybe move this elsewhere
	// ~ Server - PC and PS Valid
	void PossessedBy(AController* NewController) override;

	// ~ Client - PS Valid  
	void OnRep_PlayerState() override;

	// ~ Client - PC Valid					            
	void OnRep_Controller() override;


	
	
	
	//Delegate called when ability system initialized
	UFUNCTION()
	virtual void OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC);

	UFUNCTION()
	virtual void OnAbilitySystemUnInitialized(UDensAbilitySystemComponent* InASC);



	//Abilities
	void InitializePlayerDataAbilities();
	void RemovePlayerDataAbilities();



	// Called to bind functionality to input (from APawn)
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	
	//Perspective swap meshes
	UFUNCTION()
	void FirstToThirdPersonMeshVisibilitySwap(bool bIsFirstPerson);


	

//////////////////////////////////////////////////////////
// Components
/////////////////////////////////////////////////////////
private:

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FirstPersonRoot;


	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FPMeshRootSpringArm;
	
	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* OffsetRootComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FP_UpperArmsMesh;

	/*UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* WeaponComponent;
	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;*/


	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TP_ArmsMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TP_ChestMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TP_HeadMesh;
	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TP_ClassItem;
	


	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* RootSpringArm;

	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalBoneCamera;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;



	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDensWeaponEquipmentComponent* WeaponEquipmentComponent;

	
protected:

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsRecoilRecoveryActive = false;


	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsInFirstPerson;
	
public:
	
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; };
	USkeletalMeshComponent* GetThirdPersonArmsMesh() const { return TP_ArmsMesh; };

	void SetHeadMesh(TObjectPtr<USkeletalMesh> NewMesh);

	void SetArmsMesh(TObjectPtr<USkeletalMesh> NewMesh);
	void SetArmsFPMesh(TObjectPtr<USkeletalMesh> NewMesh);

	void SetChestMesh(TObjectPtr<USkeletalMesh> NewMesh);
	void SetChestFPMesh(TObjectPtr<USkeletalMesh> NewMesh);
	
	void SetLegsMesh(TObjectPtr<USkeletalMesh> NewMesh);

	void SetClassItemMesh(TObjectPtr<USkeletalMesh> NewMesh);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetupADSWeaponOffset();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponSwapped();

	//Used in mantle system
	UFUNCTION(BlueprintImplementableEvent)
	void OnGroundedJumpTriggered();

	

//////////////////////////////////////////////////////////////////
//Movement stuff. Gameplay feel stuff.  Move to a separate component at some point?
//////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UDensPlayerCharacterData* PlayerCharacterData;



	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HorizontalSpeedModifier = 0.8;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HorizontalSprintSpeedModifier = 0.2;
	
	bool bForwardInputPressed;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_MoveCompleted(const FInputActionValue& InputActionValue);

	void Input_Look_Mouse(const FInputActionValue& InputActionValue);
	
	void Input_JumpStart(const FInputActionValue& InputActionValue);
	
	void Input_JumpTriggered(const FInputActionValue& InputActionValue);
	void Input_JumpCompleted(const FInputActionValue& InputActionValue);

	void Input_Crouch(const FInputActionValue& InputActionValue);

	//void Input_Sprint(const FInputActionValue& InputActionValue);

	
	//Local only. Does not care if replicated over network as it only affects first person
	UFUNCTION(BlueprintImplementableEvent)
	void SmoothCrouch();

	

	

};