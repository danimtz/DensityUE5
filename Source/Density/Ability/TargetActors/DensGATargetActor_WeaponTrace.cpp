// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGATargetActor_WeaponTrace.h"

#include "DensCollisionChannels.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Weapons/DensWeaponInstance_Hitscan.h"



//Lyra
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




// Sets default values
ADensGATargetActor_WeaponTrace::ADensGATargetActor_WeaponTrace()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bDestroyOnConfirmation = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	//bTraceFromPlayerViewPoint = false;
	
}

void ADensGATargetActor_WeaponTrace::Configure(UDensWeaponInstance_Hitscan* InWeaponInstance, bool bInDebug,
	/*bool bInTraceFromPlayerViewPoint,*/ bool bInShouldProduceTargetDataOnServer)
{

	WeaponInstance = InWeaponInstance;
	bDebug = bInDebug;
	//bTraceFromPlayerViewPoint = bInTraceFromPlayerViewPoint;
	bInShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
}


// Called every frame
void ADensGATargetActor_WeaponTrace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADensGATargetActor_WeaponTrace::StartTargeting(UGameplayAbility* Ability)
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

void ADensGATargetActor_WeaponTrace::ConfirmTargetingAndContinue()
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

void ADensGATargetActor_WeaponTrace::CancelTargeting()
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

void ADensGATargetActor_WeaponTrace::StopTargeting()
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

FGameplayAbilityTargetDataHandle ADensGATargetActor_WeaponTrace::MakeTargetData(const TArray<FHitResult>& HitResults) const
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
TArray<FHitResult> ADensGATargetActor_WeaponTrace::PerformTargeting(AActor* InSourceActor)
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
	for (int32 TraceIndex = 0; TraceIndex < WeaponInstance->GetBulletsPerShot(); TraceIndex++)
	{

		const float BaseSpreadAngle = WeaponInstance->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = WeaponInstance->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector BulletDir = VRandConeNormalDistribution(AimDir, HalfSpreadAngleInRadians);/*, WeaponInstance->GetSpreadExponent());*/

		const FVector EndTrace = StartTrace + (BulletDir * WeaponInstance->GetMaxDamageRange());
		FVector HitLocation = EndTrace;

		TArray<FHitResult> AllImpacts;

		FHitResult Impact = SingleBulletTrace(StartTrace, EndTrace, WeaponInstance->GetTraceSweepRadius(), /*out*/ AllImpacts);
		ReturnHitResults.Add(Impact);
	}

	return ReturnHitResults;

}



FHitResult ADensGATargetActor_WeaponTrace::SingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, TArray<FHitResult>& OutHits) const
{




	FHitResult Impact;

	
	
	
	
	//Trace and process bullet.
	//Try single line trace first
	if(OutHits.IsEmpty())
	{
		Impact = PerformTrace(StartTrace, EndTrace, SweepRadius, /*out*/ OutHits);
	}

	//If weapon has a sweep radius and it didnt hit a pawn with line trace try sweep (TODO good place for aim assist shennanigans)
	//TODO right now this wont work if line trace hits wall and sphere trace shouldve hit an enemy
	bool bSweepFoundHits = false;
	if(!OutHits.IsEmpty())
	{
		//If first hit is not a Pawn, try to do sweep trace if supported
		if(OutHits[0].HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			if (SweepRadius > 0.0f)
			{
				TArray<FHitResult> SweepHits;
				Impact = PerformTrace(StartTrace, EndTrace, SweepRadius, /*out*/ SweepHits);

				//Process Sweep results
				//TODO
				
				if(!SweepHits.IsEmpty())
				{
					bSweepFoundHits = true;
				}
			}
		}
	}


	#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			static float DebugThickness = 0.0f;
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 2, 0, DebugThickness);
			DrawDebugPoint(GetWorld(), Impact.ImpactPoint, 16.0f, (Impact.bBlockingHit ? FColor::Red : FColor::Green), false, 2);
			
			//TODO draw debug not from camera origin
		}
	#endif


	//If no hits still add a dummy HitResult at the end of the trace
	if (OutHits.IsEmpty() && !bSweepFoundHits)
	{
			
		
		// Start param could be player ViewPoint. We want HitResult to always display the StartLocation.
		Impact.TraceStart = StartLocation.GetTargetingTransform().GetLocation();
		Impact.TraceEnd = EndTrace;
		Impact.Location = EndTrace;
		Impact.ImpactPoint = EndTrace;
		

		return Impact;
	}

	
	return Impact;
	
}



FHitResult ADensGATargetActor_WeaponTrace::PerformTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, TArray<FHitResult>& OutHitResults) const
{
	
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ OwningAbility->GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;
	//AddAdditionalTraceIgnoreActors(TraceParams);
	//TraceParams.bDebugQuery = true;

	ECollisionChannel Channel = Density_TraceChannel_Weapon;
	
	TArray<FHitResult> HitResults;
	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, Channel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, Channel, TraceParams);
	}

	
	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// Filter the output to prevent multiple hits on the same actor; TODO prioritize Weakspot hits
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
			{
				return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
			};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		Hit = OutHitResults.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
	
}



