// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGATargetActor_WeaponProjectile.h"

#include "DensCollisionChannels.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Weapons/DensWeaponInstance_Projectile.h"






// Sets default values
ADensGATargetActor_WeaponProjectile::ADensGATargetActor_WeaponProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bDestroyOnConfirmation = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	//bTraceFromPlayerViewPoint = false;
	
}

void ADensGATargetActor_WeaponProjectile::Configure(UDensWeaponInstance_Projectile* InWeaponInstance, bool bInDebug,
	/*bool bInTraceFromPlayerViewPoint,*/ bool bInShouldProduceTargetDataOnServer)
{

	WeaponInstance = InWeaponInstance;
	bDebug = bInDebug;
	//bTraceFromPlayerViewPoint = bInTraceFromPlayerViewPoint;
	bInShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
}


// Called every frame
void ADensGATargetActor_WeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADensGATargetActor_WeaponProjectile::StartTargeting(UGameplayAbility* Ability)
{
	SetActorTickEnabled(true);

	OwningAbility = Ability;
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	// This is a lazy way of emptying and repopulating the ReticleActors.
	// We could come up with a solution that reuses them.
	//DestroyReticleActors();

	/*if (ReticleClass)
	{
		for (int32 i = 0; i < MaxHitResultsPerTrace * NumberOfTraces; i++)
		{
			SpawnReticleActor(GetActorLocation(), GetActorRotation());
		}
	}*/

	/*if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}*/
}

void ADensGATargetActor_WeaponProjectile::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		TArray<FHitResult> HitResults = PerformTargeting(SourceActor);
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(HitResults);
		TargetDataReadyDelegate.Broadcast(Handle);

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			//ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForDuration, 2.0f);
		}
#endif
	}

	/*if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}*/
}

void ADensGATargetActor_WeaponProjectile::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());

	SetActorTickEnabled(false);

	/*if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}*/
}

void ADensGATargetActor_WeaponProjectile::StopTargeting()
{
	SetActorTickEnabled(false);

	//DestroyReticleActors();

	// Clear added callbacks
	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();

	if (GenericDelegateBoundASC)
	{
		GenericDelegateBoundASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
		GenericDelegateBoundASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);
		GenericDelegateBoundASC = nullptr;
	}
}

FGameplayAbilityTargetDataHandle ADensGATargetActor_WeaponProjectile::MakeTargetData(const TArray<FHitResult>& HitResults) const
{
	FGameplayAbilityTargetDataHandle ReturnDataHandle;

	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = HitResults[i];
		ReturnDataHandle.Add(ReturnData);
	}

	return ReturnDataHandle;
}


//Line trace
TArray<FHitResult> ADensGATargetActor_WeaponProjectile::PerformTargeting(AActor* InSourceActor)
{
	if (!PrimaryPC)
	{
		//TODO Cancel ability
		UE_LOG(LogTemp, Error, TEXT("Player controller nullptr instide of WeaponTrace TargetActor [%s]"),  *GetNameSafe(this));
	}


	
	
	//Starting point of trace
	FVector StartTrace = StartLocation.GetTargetingTransform().GetLocation();
	
	FVector ViewStart;
	FRotator ViewRot;
	PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);


	ADensCharacter* DensCharacter = Cast<ADensCharacter>(PrimaryPC->GetCharacter());
	if(DensCharacter)
	{
		StartTrace = DensCharacter->GetTargetingLocation();
	}
	
	/*if(bTraceFromPlayerViewPoint)
	{
		StartTrace = ViewStart;
	}*/

	

	FRotator CamRot = PrimaryPC->GetControlRotation();

	//Perfect aim direction
	FVector AimDir = CamRot.Vector();

	//End of trace with perfect aim
	//FVector EndAimPerfect = StartTrace + AimDir * WeaponInstance->GetMaxDamageRange();

	TArray<FHitResult> ReturnHitResults;
	FHitResult InitialTrace;

	//Kinda dodgy but use impact point and impact normal to send projectile spawn location and projectile direction
	InitialTrace.ImpactPoint = StartTrace;
	InitialTrace.ImpactNormal = WeaponInstance->GetProjectileDirection();

	ReturnHitResults.Add(InitialTrace);
	/*
	
	for (int32 TraceIndex = 0; TraceIndex < WeaponInstance->GetBulletsPerShot(); TraceIndex++)
	{

		const float BaseSpreadAngle = WeaponInstance->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = WeaponInstance->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector BulletDir = VRandConeNormalDistribution(AimDir, HalfSpreadAngleInRadians);

		const FVector EndTrace = StartTrace + (BulletDir * WeaponInstance->GetMaxDamageRange());
		FVector HitLocation = EndTrace;

		TArray<FHitResult> AllImpacts;

		FHitResult Impact = SingleBulletTrace(StartTrace, EndTrace, WeaponInstance->GetTraceSweepRadius(),  AllImpacts);
		ReturnHitResults.Add(Impact);
	}*/

	return ReturnHitResults;

}





