// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DensCombatReticleWidget.generated.h"

/**
 * Main reticle class that holds multiple reticles for ADS, Hipfire and Running
 */
UCLASS()
class DENSITY_API UDensCombatReticleWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetNewADSReticle(TSubclassOf<UDensWeaponReticleWidget> NewReticle);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetNewHipfireReticle(TSubclassOf<UDensWeaponReticleWidget> NewReticle);


	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSprintTagChanged(bool bIsRunning);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnADSTagChanged(bool bIsADS);



	void NewWeaponEquipped(UDensWeaponInstance* NewWeaponInstance);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnNewWeaponEquipped(UDensWeaponInstance* NewWeaponInstance);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnTargetHit();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnTargetKilled();

};
