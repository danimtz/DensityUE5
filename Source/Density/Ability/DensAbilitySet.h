// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"

#include "DensAbilitySet.generated.h"



class UAttributeSet;
class UGameplayEffect;
class UDensAbilitySystemComponent;
class UDensGameplayAbility;


/**
 * FLyraAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FDensAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDensGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};




USTRUCT(BlueprintType)
struct FDensAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	
	void RemoveAbilitiesFromASC(UDensAbilitySystemComponent* DensASC);

	//TODO move this to a DensWeaponAbilitySet? 
	//Removes perk effects from abilities that derive from DensGA_WeaponPerk and have the RemoveEffect tag set
	void RemovePerkEffectsFromASC(UDensAbilitySystemComponent* DensASC);

	void AddPerkRemoveTag(FGameplayTag PerkTag);
	
	//const TArray<FGameplayAbilitySpecHandle>& GetAbilitySpecHandles() const;

	void SaveAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	
protected:
	
	
	void ResetHandles();

protected:
	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	FGameplayTagContainer PerkRemoveTags;
	

};



/**
 * 
 */
UCLASS(BlueprintType, Const)
class DENSITY_API UDensAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:

	UDensAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	// Grants the ability set to the specified ability system component. 
	void AddToASC(UDensAbilitySystemComponent* DensASC, FDensAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr);
	


protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FDensAbilitySet_GameplayAbility> GrantedGameplayAbilities;



	//FDensAbilitySet_GrantedHandles GrantedAbilityHandles;

	//@TODO: Maybe add posibility of granting Gameplay effects and attribute sets through here instead
	// Gameplay effects to grant when this ability set is granted.
	
	// Attribute sets to grant when this ability set is granted.
	
};