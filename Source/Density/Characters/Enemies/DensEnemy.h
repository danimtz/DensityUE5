// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Characters/DensCharacterBase.h"
#include "DensEnemy.generated.h"

class UDensCombatAttributeSet;
class UDensAbilitySystemComponent;
class UDensBaseStatsAttributeSet;
class UDensEnemyData;
struct FOnAttributeChangeData;


UCLASS()
class DENSITY_API ADensEnemy : public ADensCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADensEnemy(const class FObjectInitializer& ObjectInitializer);

	
	


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Enemy Data")
	UDensEnemyData* EnemyData;

	UPROPERTY(EditDefaultsOnly, Category = "Density|Ability Sets")
	TArray<TObjectPtr<UDensAbilitySet>> AdditionalAbilitySets;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void InitializeAttributes();

	void InitializeAbilities(); //maybe this can go in characterbase?
	
	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	//Tag callbacks
	virtual void ImmuneTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	

protected:
	
	UPROPERTY()
	UDensAbilitySystemComponent* AbilitySystemComponentHardRef;
	
	UPROPERTY()
	UDensBaseStatsAttributeSet* BaseStatsAttributeSetHardRef;

	UPROPERTY()
	TObjectPtr<UDensCombatAttributeSet> CombatAttributeSet;

	
	FDelegateHandle HealthChangedDelegateHandle; //TODO might not need this delegate handle
	
	

};
