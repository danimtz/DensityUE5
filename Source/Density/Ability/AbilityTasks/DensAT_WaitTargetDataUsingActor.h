// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "DensAT_WaitTargetDataUsingActor.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataUsingActorDelegate, const FGameplayAbilityTargetDataHandle&,
                                            Data);



/**
 *	Wait TargetData from a spawned actor without destroying it when it receives data
 * 
 */
UCLASS()
class DENSITY_API UDensAT_WaitTargetDataUsingActor : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate Cancelled;

	/**
	* Uses specified spawned TargetActor and waits for it to return valid data or to be canceled. The TargetActor is *NOT* destroyed.
	*
	* @param bCreateKeyIfNotValidForMorePredicting Will create a new scoped prediction key if the current scoped prediction key is not valid for more predicting.
	* If false, it will always create a new scoped prediction key. We would want to set this to true if we want to use a potentially existing valid scoped prediction
	* key like the ability's activation key in a batched ability.
	*/
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UDensAT_WaitTargetDataUsingActor* WaitTargetDataWithReusableActor(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
		AGameplayAbilityTargetActor* InTargetActor,
		bool bCreateKeyIfNotValidForMorePredicting = false
	);

	virtual void Activate() override;

	
	UFUNCTION()
	virtual void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	UFUNCTION()
	virtual void OnTargetDataReplicatedCancelledCallback();

	UFUNCTION()
	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	virtual void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);


	/** Called when the ability is asked to confirm from an outside node. What this means depends on the individual task. By default, this does nothing other than ending if bEndTask is true. */
	virtual void ExternalConfirm(bool bEndTask) override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

protected:

	UPROPERTY()
	AGameplayAbilityTargetActor* TargetActor;

	bool bCreateKeyIfNotValidForMorePredicting;

	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

	FDelegateHandle OnTargetDataReplicatedCallbackDelegateHandle;

	virtual void InitializeTargetActor() const;
	
	virtual void FinalizeTargetActor() const;

	virtual void RegisterTargetDataCallbacks();

	virtual void OnDestroy(bool AbilityEnded) override;

	virtual bool ShouldReplicateDataToServer() const;

	
};
