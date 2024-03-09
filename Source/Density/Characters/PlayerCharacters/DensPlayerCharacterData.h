// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DensPlayerCharacterData.generated.h"

class UDensAbilitySet;
class UDensInputConfig;
/**
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Player Character Data", ShortTooltip = "Data asset used to define a Abilities and Inputs for player."))
class DENSITY_API UDensPlayerCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:

	UDensPlayerCharacterData(const FObjectInitializer& ObjectInitializer);

public:

	
	// Ability sets to grant to the players's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Abilities")
	TArray<TObjectPtr<UDensAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	//TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;


	// Input configuration to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Density|Input")
	TObjectPtr<UDensInputConfig> InputConfig;

	

};
