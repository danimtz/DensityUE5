// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensGameplayAbility.h"
#include "DensGA_WeaponPerk.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensGA_WeaponPerk : public UDensGameplayAbility
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "Ability Damage", Meta = (Categories = "Gameplay.WeaponPerks"))
	FGameplayTag RemoveEffectTag;

};
