// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAT_ServerWaitClientTargetData.h"

#include "AbilitySystemComponent.h"




UDensAT_ServerWaitClientTargetData* UDensAT_ServerWaitClientTargetData::ServerWaitClientTargetData(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, bool TriggerOnce)
{
	UDensAT_ServerWaitClientTargetData* TaskObj = NewAbilityTask<UDensAT_ServerWaitClientTargetData>(OwningAbility, TaskInstanceName);
	TaskObj->bTriggerOnce = TriggerOnce;
	return TaskObj;
	
	
}

void UDensAT_ServerWaitClientTargetData::Activate()
{
	if(!Ability || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UDensAT_ServerWaitClientTargetData::OnTargetDataReplicatedCallback);
}



void UDensAT_ServerWaitClientTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
	FGameplayTag ActivationTag)
{
	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(MutableData);
	}

	if(bTriggerOnce)
	{
		EndTask();
	}
	
}




void UDensAT_ServerWaitClientTargetData::OnDestroy(bool AbilityEnded)
{
	if(AbilitySystemComponent.IsValid())
	{
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}
	
	Super::OnDestroy(AbilityEnded);
}



