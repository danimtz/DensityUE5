// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Weapons/DensWeaponDefinition.h"

UDensWeaponDefinition::UDensWeaponDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


	ADSMovementModifier = 1.0f;
}

float UDensWeaponDefinition::GetFireDelayFromRPM() const
{

	if (RoundsPerMinute != 0)
	{
		float FireDelay = 1.0f / ( float(RoundsPerMinute) / 60.0f );

		return FireDelay;
	}

	return 0.0f;
	
}
