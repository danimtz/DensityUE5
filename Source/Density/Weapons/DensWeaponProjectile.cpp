// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeaponProjectile.h"

#include "DensWeaponInstance_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADensWeaponProjectile::ADensWeaponProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;

	bReplicates = true;

	//TODO change this to a better value
	NetUpdateFrequency = 100.0f;
}

void ADensWeaponProjectile::BeginPlay()
{

	if(InstigatorWeaponInstance)
	{
		BlastRadius = InstigatorWeaponInstance->GetProjectileBlastRadius();
		BlastDistanceDamageFalloff = InstigatorWeaponInstance->GetBlastDistanceDamageFalloff();
		ProjectileSpeed = InstigatorWeaponInstance->GetProjectileVelocity();
	}


	
	Super::BeginPlay();

	
}


float ADensWeaponProjectile::GetBlastDistanceAttenuation(float Distance)
{
	const FRichCurve* Curve = BlastDistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
}

void ADensWeaponProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION( ADensWeaponProjectile, BlastRadius,	COND_None );
	
	DOREPLIFETIME_CONDITION( ADensWeaponProjectile, ProjectileSpeed,	COND_None );
}



float ADensWeaponProjectile::GetDistanceAttenuation(float Distance) const
{
	const FRichCurve* Curve = BlastDistanceDamageFalloff.GetRichCurveConst();

	float NormalizedDistance =  Distance/BlastRadius;
	
	return Curve->HasAnyData() ? Curve->Eval(NormalizedDistance) : 1.0f;
}
