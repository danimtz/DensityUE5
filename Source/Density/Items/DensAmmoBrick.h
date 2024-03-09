// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensPickupItem.h"
#include "DensAmmoBrick.generated.h"

enum class EWeaponAmmoType : uint8;

UCLASS(Blueprintable)
class DENSITY_API ADensAmmoBrick : public ADensPickupItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADensAmmoBrick();

	
	virtual bool K2_CanBePickedUp_Implementation(ADensCharacter* TargetCharacter) const override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EWeaponAmmoType AmmoType;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BrickValue;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void ApplyItemToCharacter(ADensCharacter* TargetCharacter) override;

};
