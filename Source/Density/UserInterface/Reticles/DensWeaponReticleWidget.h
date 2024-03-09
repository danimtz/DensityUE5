// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DensWeaponReticleWidget.generated.h"

class UDensWeaponInstance;
/**
 *  Single reticle widget class. 
 */
UCLASS()
class DENSITY_API UDensWeaponReticleWidget : public UUserWidget
{
	GENERATED_BODY()

public:


	UPROPERTY(BlueprintReadWrite, VisibleAnywhere);
	UDensWeaponInstance* ActiveWeaponInstance = nullptr;
};
