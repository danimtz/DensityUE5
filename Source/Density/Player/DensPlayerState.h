// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagAssetInterface.h"
#include "DensPlayerState.generated.h"


class UDensAbilitySystemComponent;
class UAbilitySystemComponent;
class UDensCoreStatsAttributeSet;
class UDensBaseStatsAttributeSet;
class UDensAbilityEnergyAttributeSet;
class UDensBaseWeaponStatsAttributeSet;
class UDensCombatAttributeSet;
class UDensEquipmentManagerComponent;

struct FWeaponBasicAttributes;
enum class EWeaponSlot : uint8;


UCLASS()
class DENSITY_API ADensPlayerState : public APlayerState, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:

	ADensPlayerState();

	// Implement IAbilitySystemInterface
	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerState")
	UDensAbilitySystemComponent* GetDensAbilitySystemComponent() const { return AbilitySystemComponent; }

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	
	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerState")
	UDensEquipmentManagerComponent* GetEquipmentManagerComponent() const;

	
	UDensCoreStatsAttributeSet* GetCoreStatsAttributeSet() const; 
	UDensBaseStatsAttributeSet* GetBaseStatsAttributeSet() const;
	UDensAbilityEnergyAttributeSet* GetAbilityEnergyAttributeSet() const;
	
	UDensBaseWeaponStatsAttributeSet* GetEquippedWeaponAttributeSet() const;
	
	FWeaponBasicAttributes& GetSavedWeaponAttribute(EWeaponSlot Slot); //I dont like this coupling here
	
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~ End AActor interface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// Implement IAbilitySystemInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;


	
public:
	/*
	*	Attribute getters. Returns current value
	*/
	
	//TODO add all attribute getters

	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetEquippedWeaponMagAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetGrenadeEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetShield() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetHealthPercentage() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|PlayerState|Attributes")
	float GetShieldPercentage() const;

	
	void UnEquippedAmmoChanged();
	
protected:

	//~ Begin APlayerState interface
	virtual void CopyProperties(APlayerState* PlayerState);
	//~ End APlayerState interface

	UPROPERTY()
	UDensAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UDensEquipmentManagerComponent* EquipmentManagerComponent;

	
	UPROPERTY()
	UDensCoreStatsAttributeSet* CoreStatsAttributeSet;

	UPROPERTY()
	UDensBaseStatsAttributeSet* BaseStatsAttributeSet;

	UPROPERTY()
	UDensCombatAttributeSet* CombatAttributeSet;
	
	UPROPERTY()
	UDensAbilityEnergyAttributeSet* AbilityEnergyAttributeSet;

	UPROPERTY()
	UDensBaseWeaponStatsAttributeSet* EquippedWeaponAttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_SavedWeaponChanged)
	TArray<FWeaponBasicAttributes> WeaponSavedAttributes;

	//class UDensInventoryManager ? //This class should handle what weapons are availible and which of those will be placed on the WeaponEquipmentComponent

	UFUNCTION()
	virtual void OnRep_SavedWeaponChanged();

	
	FDelegateHandle HandleFullHealthBarDelegateHandle;
	FDelegateHandle ActiveMagAmmoChangedDelegateHandle;
	FDelegateHandle ActiveReserveAmmoChangedDelegateHandle;
	FDelegateHandle GrenadeEnergyChangedDelegateHandle;
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle ShieldChangedDelegateHandle;

	
	virtual void HandleFullHealthBar();
	virtual void MagAmmoChanged(const FOnAttributeChangeData& Data);
	virtual void ReserveAmmoChanged(const FOnAttributeChangeData& Data);
	virtual void GrenadeEnergyChanged(const FOnAttributeChangeData& Data);
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void ShieldChanged(const FOnAttributeChangeData& Data);
	virtual void SprintTagChanged(FGameplayTag CallbackTag, int32 NewCount);
	virtual void ADSTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION( Client, Reliable )
	void FadeOutHUDHealthBar();

	
	
	
};
