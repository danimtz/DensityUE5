// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DensDamageSource.generated.h"


UINTERFACE()
class UDensDamageSource : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DENSITY_API IDensDamageSource
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	virtual float GetDistanceAttenuation(float Distance) const = 0;
	
};
