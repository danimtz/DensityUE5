// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DensHUDWidget.generated.h"

class UDensEnemyData;
/**
 * 
 */
UCLASS()
class DENSITY_API UDensHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


	/**
	* Attribute setters
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMagAmmo(float MagAmmo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetReservesAmmo(float ReservesAmmo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTotalAmmo(float ReservesAmmo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetGrenadeEnergy(float GrenadeEnergy);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetShieldPercentage(float ShieldPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FadeOutHealthBar();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitBossHealthBar(UDensEnemyData* BossData);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetBossHealthPercentage(float HealthPercentage);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetBossHealthImmune(bool bIsImmune);



	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnNewWeaponEquipped(UDensWeaponInstance* NewWeaponInstance);

	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitNewWeaponOnHUD(UDensWeaponInstance* NewWeaponInstance);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUnequippedAmmo(const TArray<FWeaponBasicAttributes>& Array);
};
	