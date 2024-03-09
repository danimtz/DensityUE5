// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeaponInstance_Hitscan.h"

#include "Ability/TargetActors/DensGATargetActor_WeaponTrace.h"


ADensGATargetActor_WeaponTrace* UDensWeaponInstance_Hitscan::GetTraceTargetActor()
{
	if (TargetActor)
	{
		return Cast<ADensGATargetActor_WeaponTrace>(TargetActor);
	}

	TargetActor = GetWorld()->SpawnActor<ADensGATargetActor_WeaponTrace>();
	//TraceTargetActor->SetOwner(this);
	return Cast<ADensGATargetActor_WeaponTrace>(TargetActor);
}


