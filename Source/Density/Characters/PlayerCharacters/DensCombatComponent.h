// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DensCombatComponent.generated.h"


struct FGameplayEffectSpec;
class UDensAbilitySystemComponent;
class UDensCombatAttributeSet;


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDensHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDensCombat_AttributeEvent, AActor*, EffectInstigator, AActor* ,EffectCauser, const FGameplayEffectSpec& , EffectSpec, float , EffectMagnitude);



UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class DENSITY_API UDensCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensCombatComponent();

	// Returns the combat component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Density|Health")
	static UDensCombatComponent* FindCombatComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDensCombatComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Density | Combat")
	void OnAbilitySystemComponentInitialized(UDensAbilitySystemComponent* InASC);

	
	
	
	// Attribute Getters
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetMaxShield() const;

	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetShield() const;
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetCritDamageModifier() const;

	UFUNCTION(BlueprintCallable, Category= "Density|Combat Attributes")
	float GetHealthPercentage() const;

	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Attributes")
	float GetStaggerThreshold() const;

	UFUNCTION(BlueprintCallable, Category= "Density|Combat Attributes")
	float GetCurrentStagger() const;

	
	//Damage Meta attribute get and setters

	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetOutgoingDamage();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetOutgoingDamage(float NewValue);


	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetOutgoingStaggerDamage();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetOutgoingStaggerDamage(float NewValue);
	
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetOutgoingEmpowerBuff();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetOutgoingEmpowerBuff(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetOutgoingSurgeBuff();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetOutgoingSurgeBuff(float NewValue);

	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetIncomingDamage();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetIncomingDamage(float NewValue);


	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetIncomingStaggerDamage();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetIncomingStaggerDamage(float NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetIncomingWeakenDebuff();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetIncomingWeakenDebuff(float NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	float GetIncomingDamageResistBuff();
	
	UFUNCTION(BlueprintCallable, Category = "Density|Combat Meta Attributes")
	void SetIncomingDamageResistBuff(float NewValue);

	



	
public:

	//TODO delegates to send out to other classes when changes from from attribute set received
	
	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FDensCombat_AttributeEvent OnOutOfHealth;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FDensCombat_AttributeEvent OnOutOfShield;

	UPROPERTY(BlueprintAssignable)
	FDensCombat_AttributeEvent OnDamageTaken;

	UPROPERTY(BlueprintAssignable)
	FDensCombat_AttributeEvent OnStaggerThresholdCrossed;
	/*
	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_DeathEvent OnDeathFinished;
	*/
	
	
protected:

	UFUNCTION()
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void HandleStaggerThresholdCrossed(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);
	

protected:



	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UDensAbilitySystemComponent> DensASC;

	// Combat attribute set (Owned by player state in character)
	UPROPERTY()
	TObjectPtr<const UDensCombatAttributeSet> CombatAttributeSet;

	
};


