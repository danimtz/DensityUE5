// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAmmoBrick.h"

#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"


// Sets default values
ADensAmmoBrick::ADensAmmoBrick()
{
	
	
}

bool ADensAmmoBrick::K2_CanBePickedUp_Implementation(ADensCharacter* TargetCharacter) const
{

	return !TargetCharacter->GetWeaponEquipmentComponent()->IsAmmoTypeFull(AmmoType);
	
}


void ADensAmmoBrick::BeginPlay()
{
	Super::BeginPlay();
	
}



void ADensAmmoBrick::ApplyItemToCharacter(ADensCharacter* TargetCharacter)
{
	
	TargetCharacter->GetWeaponEquipmentComponent()->OnAmmoBrickPickedUp(AmmoType, BrickValue);
	
	Super::ApplyItemToCharacter(TargetCharacter);
}



