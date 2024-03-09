// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensCombatReticleWidget.h"

#include "Weapons/DensWeaponInstance.h"

void UDensCombatReticleWidget::NewWeaponEquipped(UDensWeaponInstance* NewWeaponInstance)
{

	SetNewHipfireReticle(NewWeaponInstance->GetHipfireReticleClass());
	SetNewADSReticle(NewWeaponInstance->GetADSReticleClass());
	
	OnNewWeaponEquipped(NewWeaponInstance);
}

