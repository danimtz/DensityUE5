// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DensEnemyData.generated.h"

class UDensAbilitySet;

UENUM(BlueprintType)
enum class EEnemyTier : uint8
{
	Minor,
	Major,
	Miniboss,
	Boss,
	BossDummy
};


USTRUCT(Blueprintable)
struct FStaggerAnimations
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  HeavyStagger_Back;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  HeavyStagger_Front;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  HeavyStagger_Right;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  HeavyStagger_Left;
	
};


class UDensStatusBarWidget;
/**
 * 
 */
UCLASS()
class DENSITY_API UDensEnemyData : public UDataAsset
{
	GENERATED_BODY()

public:
	UDensEnemyData(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Name")
	FText EnemyName;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Health")
	float Health;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Health")
	float StaggerThreshold;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Stats")
	float MaxMovespeed = 600;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy CritDamageModifier")
	float CritDamageModifier;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Type")
	EEnemyTier EnemyTier;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Status Bar")
	TSubclassOf<UDensStatusBarWidget> StatusBarClass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Abilities")
	TObjectPtr<UDensAbilitySet> DefaultAbilitySet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemies", Meta = (Categories = "Gameplay.Faction"))
	FGameplayTag Faction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemies", Meta = (Categories = "Gameplay.Faction"))
	FGameplayTagContainer EnemyFactions;



	//Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  SpawnAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	TObjectPtr<UAnimMontage>  DeathAnimation;


	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Animations")
	FStaggerAnimations  HitReactAnimations;



};
