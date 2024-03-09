// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "DensAT_ServerWaitClientTargetData.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensAT_ServerWaitClientTargetData : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate ValidData;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UDensAT_ServerWaitClientTargetData* ServerWaitClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool TriggerOnce);


	virtual void Activate() override;

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	
protected:
	virtual void OnDestroy(bool AbilityEnded) override;

	bool bTriggerOnce;
};
