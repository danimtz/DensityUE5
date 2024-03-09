// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Ability/DensAbilityTypes.h"
#include "Ability/DensDamageSource.h"
#include "GameFramework/Actor.h"
#include "DensWeaponProjectile.generated.h"

class UDensWeaponInstance_Projectile;

UCLASS()
class DENSITY_API ADensWeaponProjectile : public AActor, public IDensDamageSource
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADensWeaponProjectile();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	float GetBlastDistanceAttenuation(float Distance);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual float GetDistanceAttenuation(float Distance) const override;
	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovement;

	//Manual value will be ignored if a valid WeaponInstance is given at creation and will use value from weapon instance instead
	UPROPERTY(BlueprintReadOnly, Replicated, VisibleAnywhere, Category = "Projectile")
	float ProjectileSpeed = 5000.0f;

	//Manual value will be ignored if a valid WeaponInstance is given at creation and will use value from weapon instance instead
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Projectile")
	float BlastRadius = 200.0f;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "WeaponInstance",  meta = (ExposeOnSpawn="true"))
	UDensWeaponInstance_Projectile* InstigatorWeaponInstance;

	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Projectile",  meta = (ExposeOnSpawn="true"))
	FDensDamageContainerSpec ImpactDamageContainer;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Projectile",  meta = (ExposeOnSpawn="true"))
	FDensDamageContainerSpec BlastDamageContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	FRuntimeFloatCurve BlastDistanceDamageFalloff;


};
