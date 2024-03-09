// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "DensGATargetActor_WeaponTrace.generated.h"

class UDensWeaponInstance_Hitscan;

UCLASS()
class DENSITY_API ADensGATargetActor_WeaponTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ADensGATargetActor_WeaponTrace();

	
	/**
	* Configure the TargetActor for use. This TargetActor could be used in multiple abilities and there's no guarantee
	* what state it will be in. You will need to make sure that only one ability is using this TargetActor at a time.
	*
	* @param WeaponInstance Weapon instance from which to fire the bullet. Contains values for spread etc
	* @param bInDebug When true, this TargetActor will show debug lines of the trace and hit results.
	* @param bInTraceAffectsAimPitch Does the trace affect the aiming pitch?
	* @param bInTraceFromPlayerViewPoint Should we trace from the player ViewPoint instead of the StartLocation? The
	* TargetData HitResults will still have the StartLocation for the TraceStart. This is useful for FPS where we want
	* to trace from the player ViewPoint but draw the bullet tracer from the weapon muzzle.
	* @param bInShouldProduceTargetDataOnServer If set, this TargetActor will produce TargetData on the Server in addition
	* to the client and the client will just send a generic "Confirm" event to the server. If false, the client will send
	* the TargetData to the Server. This is handled by the WaitTargetDataUsingActor AbilityTask.
	* TODO: AI Controllers should fall back to muzzle location. Not implemented yet.
	**/
	UFUNCTION(BlueprintCallable)
	void Configure(
		UPARAM(DisplayName = "WeaponInstance") UDensWeaponInstance_Hitscan* WeaponInstance,
		UPARAM(DisplayName = "Debug") bool bInDebug = false,
		//UPARAM(DisplayName = "Trace From Player ViewPoint") bool bInTraceFromPlayerViewPoint = false,
		UPARAM(DisplayName = "Should Produce Target Data on Server") bool bInShouldProduceTargetDataOnServer = false
		//UPARAM(DisplayName = "Use Persistent Hit Results") bool bInUsePersistentHitResults = false,
	);



	
	
	//
	UPROPERTY(BlueprintReadWrite, Category = "WeaponInstance")
	UDensWeaponInstance_Hitscan* WeaponInstance;
	
	
	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = "Trace")
	ECollisionChannel TraceChannel;
	*/
	/*
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bIgnoreBlockingHits;
	*/
	
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	//bool bTraceFromPlayerViewPoint;

	
	/*
	// HitResults will persist until Confirmation/Cancellation or until a new HitResult takes its place
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bUsePersistentHitResults;
	*/
	
public:


	virtual void Tick(float DeltaTime) override;
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void CancelTargeting() override;


	virtual void StopTargeting();


protected:
protected:

	virtual FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<FHitResult>& HitResults) const;

	//Perform targeting
	virtual TArray<FHitResult> PerformTargeting(AActor* InSourceActor);

	FHitResult SingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, OUT TArray<FHitResult>& OutHits) const;

	FHitResult PerformTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, OUT TArray<FHitResult>& OutHitResults) const;

};
