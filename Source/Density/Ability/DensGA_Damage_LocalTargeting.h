// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensGameplayAbility_Damage.h"
#include "DensGA_Damage_LocalTargeting.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensGA_Damage_LocalTargeting : public UDensGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	
	//~UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface


	UFUNCTION(BlueprintCallable, Category = "DamageContainers")
	void ProcessLocalTargeting(const TArray<FHitResult>& HitResults);


	UFUNCTION(BlueprintImplementableEvent, Category = "DamageContainers|ClientServerTargeting")
	void OnDamageTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

protected:
	
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ApplicationTag);

	FDelegateHandle OnTargetDataReadyDelegateHandle;
};
