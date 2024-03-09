// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DensPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API ADensPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()


public:


	virtual UCameraShakeBase* StartCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale=1.f, ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal, FRotator UserPlaySpaceRot = FRotator::ZeroRotator) override;

};
