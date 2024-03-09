// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeaponInstance_Projectile.h"

#include "Ability/TargetActors/DensGATargetActor_WeaponProjectile.h"
#include "Characters/PlayerCharacters/DensCharacter.h"


static FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent = 1.0f)
{
	if (ConeHalfAngleRad > 0.f)
	{
		const float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

		// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
		// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
		const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
		const float AngleFromCenter = FromCenter * ConeHalfAngleDegrees;
		const float AngleAround = FMath::FRand() * 360.0f;

		FRotator Rot = Dir.Rotation();
		FQuat DirQuat(Rot);
		FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
		FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
		FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;
		FinalDirectionQuat.Normalize();

		return FinalDirectionQuat.RotateVector(FVector::ForwardVector);
	}
	else
	{
		return Dir.GetSafeNormal();
	}
}



ADensGATargetActor_WeaponProjectile* UDensWeaponInstance_Projectile::GetTraceTargetActor()
{
	if (TargetActor)
	{
		return Cast<ADensGATargetActor_WeaponProjectile>(TargetActor);
	}

	TargetActor = GetWorld()->SpawnActor<ADensGATargetActor_WeaponProjectile>();
	//TraceTargetActor->SetOwner(this);
	return Cast<ADensGATargetActor_WeaponProjectile>(TargetActor);
}





float UDensWeaponInstance_Projectile::GetProjectileBlastDamage() const
{
	return ProjectileBlastDamage;
}

float UDensWeaponInstance_Projectile::GetProjectileBlastStaggerDamage() const
{
	return ProjectileBlastStaggerDamage;
}


FVector UDensWeaponInstance_Projectile::GetProjectileDirection()
{

	AController* PrimaryPC = DensCharacterOwner->GetController();
	FRotator CamRot = PrimaryPC->GetControlRotation();

	//Perfect aim direction
	FVector AimDir = CamRot.Vector();

	
	const float BaseSpreadAngle = GetCalculatedSpreadAngle();
	const float SpreadAngleMultiplier = GetCalculatedSpreadAngleMultiplier();
	const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

	const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

	const FVector BulletDir = VRandConeNormalDistribution(AimDir, HalfSpreadAngleInRadians);/*, WeaponInstance->GetSpreadExponent());*/

	
	return BulletDir;

	
	
}



