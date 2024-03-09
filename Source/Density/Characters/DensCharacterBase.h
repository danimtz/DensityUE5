// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Ability/DensAbilitySet.h"
#include "GameFramework/Character.h"
#include "DensCharacterBase.generated.h"

class UDensCombatComponent;
class UDensStatusEffectsComponent;
class UDensAbilitySystemComponent;
class UDensBaseStatsAttributeSet;
struct FGameplayEffectSpec;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemInitializedDelegate, UDensAbilitySystemComponent* /* DensASC */)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilitySystemUnInitializedDelegate, UDensAbilitySystemComponent* /* DensASC */)



USTRUCT(BlueprintType)
struct DENSITY_API FCharacterDeathInfo
{
	GENERATED_BODY()
	
public:
	FCharacterDeathInfo() {}
	FCharacterDeathInfo(AActor* DamageInstigator, const FGameplayEffectSpec& DamageEffectSpec);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	FText DeathCauserName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayEffectContainer)
	FText DeathAbilityName;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterDiedDelegate, ADensCharacterBase*, Character, FCharacterDeathInfo, DeathInfo);





/** Interface for assets which contain gameplay tags that represent enemy and own faction*/
UINTERFACE(BlueprintType, MinimalAPI, NotBlueprintable)
class UEnemyFactionInterface : public UInterface
{
	GENERATED_BODY()
};

class IEnemyFactionInterface 
{
	GENERATED_BODY()

public:

	/**
	 * Get Gameplay Tag representing the owners faction
	 * 
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = EnemyFaction)
	virtual FGameplayTag GetOwnFaction() const = 0;

	/**
	 * Get Gameplay Tag representing the owners enemies factions
	 * 
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = EnemyFaction)
	virtual FGameplayTagContainer GetEnemyFactions() const = 0;
	
};


UCLASS()
class DENSITY_API ADensCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IEnemyFactionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADensCharacterBase(const class FObjectInitializer& ObjectInitializer);
	
	// Implement IAbilitySystemInterface
	UFUNCTION(BlueprintCallable, Category = "Density | Character")
	UDensAbilitySystemComponent* GetDensAbilitySystemComponent() const;
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Implement IAbilitySystemInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;



	
	// Delegate fired when our character becomes the ability system's avatar actor 
	FOnAbilitySystemInitializedDelegate OnAbilitySystemInitializedDelegate;
	
	FOnAbilitySystemUnInitializedDelegate OnAbilitySystemUnInitializedDelegate;

	
	UDensBaseStatsAttributeSet* GetBaseStatsAttributeSet() const;

	
	UPROPERTY(BlueprintAssignable, Category = "Density | Character")
	FCharacterDiedDelegate OnCharacterDied;

	
	UFUNCTION(BlueprintCallable, Category = "Density|UI")
	UDensStatusBarComponent* GetStatusBar() const;
	
	

	// Attribute Getters
	UFUNCTION(BlueprintCallable, Category = "Density|Combat")
	UDensCombatComponent* GetCombatComponent() const;

	
	
	UFUNCTION(BlueprintCallable, Category = "Density | Character")
	TArray<USkeletalMeshComponent*> GetCharacterMeshes() const { return CharacterMeshes;}


	
	//Getters for attributes 
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Attributes")
	float GetWalkSpeedMod() const;

	// Gets the Current value of SprintSpeed
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Attributes")
	float GetSprintSpeedMod() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Attributes")
	float GetWalkSpeed() const;

	// Gets the Current value of SprintSpeed
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Attributes")
	float GetSprintSpeed() const;

	
	// Getters for CMC stuff
	UFUNCTION(BlueprintCallable, Category = "Density|DensCharacter|Movement")
	bool IsSprinting() const;
	

	//For animation purposes on blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void SprintStarted();

	UFUNCTION(BlueprintImplementableEvent)
	void SprintStopped();


	UFUNCTION(BlueprintCallable, Category = EnemyFaction)
	virtual FGameplayTag GetOwnFaction() const override {return Faction;};
	
	UFUNCTION(BlueprintCallable, Category = EnemyFaction)
	virtual FGameplayTagContainer GetEnemyFactions() const override {return EnemyFactions;};
	
	
	UFUNCTION(BlueprintCallable, Category = "Density | Character")
	void Die();


	virtual void Reset() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	//TODO These handle functions could go on a component
	UFUNCTION()
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleStaggerThresholdCrossed(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);


	UFUNCTION(BlueprintImplementableEvent, Category = "Density | Character")
	void OnDyingTagAdded();
	
	void DeathStarted(FCharacterDeathInfo DeathInfo);

	UFUNCTION(BlueprintCallable, Category = "Density | Character")
	void DeathFinished();
	
	void DestroyDueToDeath();
	
	virtual void DyingTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	void UninitializeAbilitySystem();
	
	
protected:
	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDensStatusEffectsComponent> StatusEffectsComponent;

	UPROPERTY()
	TWeakObjectPtr<class UDensAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TWeakObjectPtr<class UDensBaseStatsAttributeSet> BaseStatsAttributeSet;


	UPROPERTY()
	FDensAbilitySet_GrantedHandles GrantedAbilityHandles;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Denstiy|UI")
	TObjectPtr<class UDensStatusBarComponent> StatusBarWidgetComponent;


	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDensCombatComponent> CombatComponent;

	
	TArray<USkeletalMeshComponent*> CharacterMeshes;

	//CMC
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	class UDensCharacterMovementComponent* DensCharacterMovementComponent;



	//Stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Movement")
	float MaxWalkSpeed = 600.0f;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Movement")
	float MaxSprintSpeed = 1000.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Faction", Meta = (Categories = "Gameplay.Faction"))
	FGameplayTagContainer EnemyFactions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Faction")
	FGameplayTag Faction;

};
