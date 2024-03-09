// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeaponInstance.h"

#include "DensGameplayTags.h"
#include "Ability/TargetActors/DensGATargetActor_WeaponTrace.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensCharacterMovementComponent.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"

// Sets default values
UDensWeaponInstance::UDensWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	

	HeatToHeatPerShotCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	HeatToCoolDownPerSecondCurve.EditorCurveData.AddKey(0.0f, 2.0f);
	KickbackPerShotInSprayCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	KickbackToCoolDownPerSecondCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	RecoilToHeatCurve.EditorCurveData.AddKey(0.0f, 0.5f);
	
}

void UDensWeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World);
	TimeLastFired = World->GetTimeSeconds();
}




float UDensWeaponInstance::GetWeaponSpreadNormalizedValue() 
{

	float MinValue;
	float MaxValue;
	ComputeSpreadRange(MinValue, MaxValue);

	float MinHeat;
	float MaxHeat;
	ComputeHeatRange(MinHeat, MaxHeat);
	//Only apply multiplier if weapon has been fired/ there is heat
	float CurrentMultiplier = (CurrentHeat > MinHeat) ? CurrentSpreadAngleMultiplier : 1.0f;
	float CurrentSpread = CurrentSpreadAngle * CurrentMultiplier;

	float NormalizedSpread = FMath::GetMappedRangeValueClamped(FVector2d(MinValue, MaxValue), FVector2d(0.0f, 1.0f), CurrentSpread);
	return NormalizedSpread;
}


// Called every frame
void UDensWeaponInstance::UpdateWeaponOnTick(float DeltaTime)
{
	const bool bMinSpread = UpdateSpread(DeltaTime);
	const bool bMinMultipliers = UpdateMultipliers(DeltaTime);

	UpdateKickback(DeltaTime);
	
	
	if(SprayShotCount != 0)
	{
		TryResetShotCount();
	}
	
	
	
	
	if(DensCharacterOwner && bIsEquipped)
	{
		//Kickback transform
		FTransform Kickback = CalculateKickbackTransform();
		DensCharacterOwner->ApplyAnimationRecoil(Kickback);

		TryResetCamRecoil();
	}
}




void UDensWeaponInstance::AddSpread()
{
	// Sample heat up curve
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);

	
	// Map the heat to the spread angle
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);

}



bool UDensWeaponInstance::CanFireWeaponRPM() const
{

	float FireDelay = GetFireDelay();
	
	float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);

	
	if(FireDelay >  0.0f)
	{
		if(TimeLastFired == 0.0f)
		{
			return true;
		}
		
		return TimeSinceFired > FireDelay;
	}
	
	
	UE_LOG(LogTemp, Warning, TEXT("Fire delay returned 0 in Weapon instance"),  *GetNameSafe(this));

	return false;
}


FRotator UDensWeaponInstance::ApplyRecoilAndSpread()
{

	//Update firing variables
	UpdateFiringTime();
	SprayShotCount += 1;
	
	if(DensCharacterOwner)
	{
		DensCharacterOwner->SetIsRecoilRecoveryActive(false);
	}
	
	
	//Apply spread
	AddSpread();

	//Apply recoil
	//Part of this has to be in Character since recoil camera recovery is character dependant not weaponn dependant

	return ApplyRecoil();
	
}

void UDensWeaponInstance::DEBUG_PrintInstanceVariables()
{

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Current Heat: %f"), CurrentHeat));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Current Spread Angle: %f"), CurrentSpreadAngle * CurrentSpreadAngleMultiplier));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Shot Count: %d"), SprayShotCount));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Cumulative Kickback: %f"), CumulativeKickback));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("_______________________________ \n")));
	}
		
}



float UDensWeaponInstance::GetFireDelay() const
{

	//If weapon is currently equipped then obtain data from attribute set for GAS interaction
	if(bIsEquipped)
	{
		return WeaponEquipmentComponentOwner->GetFireDelay();
	}
	
	if (RoundsPerMinute != 0)
	{
		float FireDelay = 1.0f / ( float(RoundsPerMinute) / 60.0f );

		return FireDelay;
	}

	return 0.0f;
	
}


float UDensWeaponInstance::GetWeaponDamage() const
{
	return WeaponDamage;
}

float UDensWeaponInstance::GetWeaponStaggerDamage() const
{
	return WeaponStaggerDamage;
}


FGameplayTag UDensWeaponInstance::GetFireGameplayCueTag() const
{
	return WeaponFireGameplayCue;
}


/*
void UDensWeaponInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TraceTargetActor)
	{
		TraceTargetActor->Destroy();
	}
	Super::EndPlay(EndPlayReason);
}
*/

UWorld* UDensWeaponInstance::GetWorld() const
{

	if(DensCharacterOwner)
	{
		return DensCharacterOwner->GetWorld();
	}

	return nullptr;
}


float UDensWeaponInstance::GetDistanceAttenuation(float Distance) const
{

	const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
	
}


void UDensWeaponInstance::BeginDestroy()
{
	UObject::BeginDestroy();

	if (TargetActor)
	{
		TargetActor->Destroy();
	}

	
}


/*
ADensGATargetActor_WeaponTrace* UDensWeaponInstance::GetTraceTargetActor()
{
	if (TraceTargetActor)
	{
		return TraceTargetActor;
	}

	TraceTargetActor = GetWorld()->SpawnActor<ADensGATargetActor_WeaponTrace>();
	//TraceTargetActor->SetOwner(this);
	return TraceTargetActor;
}
*/




FRotator UDensWeaponInstance::ApplyRecoil()
{

	int RecoilDir = VerticalRecoilDirection;
	if(bIsEquipped)
	{
		//If equipped use recoil direction values from Attribute set (Maybe redundant since this function should not be called with weapon unequipped)
		RecoilDir =  WeaponEquipmentComponentOwner->GetShotRecoilDirection();
	}
	
	//Calculate Weapon kickback transform
	FTransform WeaponRecoilKickback = CalculateRecoilShakeTransform();


	//Apply kickback
	if(DensCharacterOwner)
	{
		DensCharacterOwner->ApplyAnimationRecoil(WeaponRecoilKickback);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DensWeaponInstance: CharacterOwner is NULL for instance: [%s], when trying to apply weapon kickback"), *GetNameSafe(this));
	}


	
	// TODO recoil pitch yaw controller

	//Calculate range from recoil direction stat

	//Calculate pitch yaw from direction of recoil

	//Recoil direction angle -> y = 100-x;
	float HalfAngleVariance = (100.0f - RecoilDir)*0.65f;

	float RecoilAngleDirection = ComputeRecoilAngleDirection(RecoilDir);
	
	
	float ShotDirection = FMath::FRandRange(RecoilAngleDirection - HalfAngleVariance, RecoilAngleDirection + HalfAngleVariance);
	
	
	float RecoilStrength = RecoilToHeatCurve.GetRichCurveConst()->Eval(CurrentHeat); //TODO Change this to a value based on stability ETC

	
	float YawRotation = RecoilStrength * FMath::Sin(FMath::DegreesToRadians(ShotDirection));
	float PitchRotation = RecoilStrength * FMath::Cos(FMath::DegreesToRadians(ShotDirection));
	FMath::Clamp(PitchRotation, 0, RecoilStrength);
	
	FRotator RecoilCamRotator = FRotator( PitchRotation, YawRotation, 0.0f);
	
	return RecoilCamRotator;
}




bool UDensWeaponInstance::UpdateSpread(float DeltaSeconds)
{

	const float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);

	if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	{
		const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
		CurrentHeat = ClampHeat(CurrentHeat - (CooldownRate * DeltaSeconds));
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	}
	
	float MinSpread;
	float MaxSpread;
	ComputeSpreadRange(/*out*/ MinSpread, /*out*/ MaxSpread);

	return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
	
}




//Look at Lyra version in the future
bool UDensWeaponInstance::UpdateMultipliers(float DeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 0.05f;

	

	if(DensCharacterOwner)
	{
		//TODO Add multipliers for In Air

	
		// Determine if we are in the air (jumping/falling),
		UDensCharacterMovementComponent* CharMovementComp = Cast<UDensCharacterMovementComponent>(DensCharacterOwner->GetMovementComponent());

		//TODO Aerial effectiveness value XDDDD 
		const bool bIsJumpingOrFalling = (CharMovementComp != nullptr) && CharMovementComp->IsFalling();
		const float JumpFallMultiplier = bIsJumpingOrFalling ? SpreadAngleModifier_InAir : 1.0f;
		const bool bJumpFallMultiplerIs1 = FMath::IsNearlyEqual(JumpFallMultiplier, 1.0f, MultiplierNearlyEqualThreshold);


	
		//Determine if we are aiming down sights
		float AimingAlpha = DensCharacterOwner->HasMatchingGameplayTag(DensGameplayTags::State_Movement_WeaponADS) ? 1.0f : 0.0f;

		bIsADS = AimingAlpha == 1.0f ? true : false; 
	
		const float AimingMultiplier = FMath::GetMappedRangeValueClamped(
			/*InputRange=*/ FVector2D(0.0f, 1.0f),
			/*OutputRange=*/ FVector2D(1.0f, SpreadAngleModifier_Aiming),
			/*Alpha=*/ AimingAlpha);
		const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleModifier_Aiming, KINDA_SMALL_NUMBER);

		// Combine all the multipliers
		const float CombinedMultiplier = AimingMultiplier * JumpFallMultiplier;
		CurrentSpreadAngleMultiplier = CombinedMultiplier;

		// need to handle these spread multipliers indicating we are not at min spread
		return bAimingMultiplierAtTarget && bJumpFallMultiplerIs1;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter not found in WeaponInstance function %s for instance %s"), *FString(__FUNCTION__), *GetName());
		return false;
	}
	
}



void UDensWeaponInstance::UpdateKickback(float DeltaSeconds)
{
	
	const float CooldownRate = KickbackToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CumulativeKickback);
	CumulativeKickback = ClampKickback(CumulativeKickback - (CooldownRate * DeltaSeconds));
	 
	
}

void UDensWeaponInstance::TryResetCamRecoil()
{
	
	const float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);

	float StartDelay = (RecoilRecoveryStartDelay == -1.0f) ? GetFireDelay()*1.1f : RecoilRecoveryStartDelay;
	
	//Check for recoil recovery start
	if (TimeSinceFired > StartDelay)
	{
		DensCharacterOwner->SetIsRecoilRecoveryActive(true);
	}

	
}


void UDensWeaponInstance::TryResetShotCount()
{

	const float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);

	//UE_LOG(LogTemp, Warning, TEXT("Time since fired: %f"), TimeSinceFired);
	//UE_LOG(LogTemp, Warning, TEXT("FireDelay: %f"), GetFireDelay());
	
	if (TimeSinceFired > (GetFireDelay() * 1.25f))
	{
		SprayShotCount = 0;
	}
	
	
}



void UDensWeaponInstance::ComputeSpreadRange(float& MinSpread, float& MaxSpread)
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(/*out*/ MinSpread, /*out*/ MaxSpread);
}

void UDensWeaponInstance::ComputeHeatRange(float& MinHeat, float& MaxHeat)
{
	float Min1;
	float Max1;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min1, /*out*/ Max1);

	float Min2;
	float Max2;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min2, /*out*/ Max2);

	float Min3;
	float Max3;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min3, /*out*/ Max3);

	MinHeat = FMath::Min(FMath::Min(Min1, Min2), Min3);
	MaxHeat = FMath::Max(FMath::Max(Max1, Max2), Max3);
}



float UDensWeaponInstance::ComputeRecoilAngleDirection(float RecoilDirectionStat)
{

	RecoilDirectionStat = FMath::Clamp(RecoilDirectionStat, 0.0f, 100.0f);
	
	// \frac{\sin\left(\left(x+5\right)\cdot\frac{\pi}{10.0}\right)\left(100-x\right)}{2}   ON DESMOS
	const float AngleDir = ((FMath::Sin( (RecoilDirectionStat+5.0f) * (UE_PI/10.0f) ) * (100.0f-RecoilDirectionStat)) * 0.5f);

	return AngleDir;
}



FTransform UDensWeaponInstance::CalculateKickbackTransform()
{
	float KickbackY = bIsADS ? 0.0f : (CumulativeKickback * -1.0f);
	
	return FTransform(FVector(0, KickbackY, 0));
}

FTransform UDensWeaponInstance::CalculateRecoilShakeTransform()
{

	//Up Down sway
	float UpDownSway = FMath::FRandRange(UpDownTranslationRange.X, UpDownTranslationRange.Y);
	UpDownSway = bIsADS ? 0.0f : UpDownSway;
	
	//Sideways sway
	float SideSway = FMath::FRandRange(SidewaysTranslationRange.X, SidewaysTranslationRange.Y);
	SideSway = bIsADS ? 0.0f : SideSway;

	//Pitch
	float PitchSway = FMath::FRandRange(PitchRotationRange.X, PitchRotationRange.Y);
	PitchSway = bIsADS ? 0.0f : PitchSway;

	//Roll
	float RollSway = FMath::FRandRange(RollRotationRange.X, RollRotationRange.Y);
	RollSway = bIsADS ? 0.0f : RollSway;
	
	//Yaw
	float YawSway = FMath::FRandRange(YawRotationRange.X, YawRotationRange.Y);
	YawSway = bIsADS ? 0.0f : YawSway;
	

	//Kickback (TODO curve shenannigans think 360 rpm vs 900 rpm kickback curve)
	
	const float CurrentShotKickback = KickbackPerShotInSprayCurve.GetRichCurveConst()->Eval(SprayShotCount);
	CumulativeKickback += CurrentShotKickback;

	FTransform Kickback = CalculateKickbackTransform();

	//Roll and pitch are swapped in the rotator due to weapon rotation in ABP?
	if(bIsADS)
	{
		return FTransform( FRotator( RollSway, YawSway, PitchSway), FVector(SideSway, (ADSKickback * -1.0f), UpDownSway));
	}
	else
	{
		return FTransform(FRotator( RollSway, YawSway, PitchSway), FVector(SideSway, Kickback.GetTranslation().Y, UpDownSway));
	}
		
	
	
	
}

