// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Ability/DensAbilitySet.h"
#include "GameFramework/Actor.h"
#include "DensDamagableActor.generated.h"

class UDensCombatComponent;
class UDensCombatAttributeSet;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemInitializedDelegate, UDensAbilitySystemComponent* /* DensASC */)


UCLASS()
class DENSITY_API ADensDamagableActor : public AActor, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADensDamagableActor(const class FObjectInitializer& ObjectInitializer);
	
	// Implement IAbilitySystemInterface
	UFUNCTION(BlueprintCallable, Category = "Density | Character")
	UDensAbilitySystemComponent* GetDensAbilitySystemComponent() const;
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	
	FOnAbilitySystemInitializedDelegate OnAbilitySystemInitializedDelegate;

	
	// Implement IAbilitySystemInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void BeginPlay() override;

public:
	
	UPROPERTY(Category = "Density", BlueprintReadWrite, meta = (AllowPrivateAccess = "true" , ExposeOnSpawn = "true"))
	float MaxHealth = 100.0f;
	
	UPROPERTY(Category = "Density", BlueprintReadWrite, meta = (AllowPrivateAccess = "true" , ExposeOnSpawn = "true"))
	float MaxShield = 0.0f;

	
protected:

	void InitializeAttributes();
	void InitializeAbilities();

	UFUNCTION()
	void OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC);
	
	UFUNCTION()
	virtual void HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	
protected:
	
	UPROPERTY()
	TObjectPtr<UDensAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDensCombatComponent> CombatComponent;
	
	UPROPERTY()
	TObjectPtr<UDensCombatAttributeSet> CombatAttributeSet;

	
	UPROPERTY(Category = "Density", EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDensAbilitySet> AbilitySet;

	
	UPROPERTY()
	FDensAbilitySet_GrantedHandles GrantedAbilityHandles;

	
};
