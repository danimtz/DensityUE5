// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAT_WaitTargetDataUsingActor.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Ability/TargetActors/DensGATargetActor_WeaponTrace.h"


UDensAT_WaitTargetDataUsingActor::UDensAT_WaitTargetDataUsingActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


UDensAT_WaitTargetDataUsingActor* UDensAT_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor, bool bCreateKeyIfNotValidForMorePredicting)
{
	//Register for task list here, providing a given FName as a key
	UDensAT_WaitTargetDataUsingActor* MyObj = NewAbilityTask<UDensAT_WaitTargetDataUsingActor>(OwningAbility, TaskInstanceName);		
	MyObj->TargetActor = InTargetActor;
	MyObj->ConfirmationType = ConfirmationType;
	MyObj->bCreateKeyIfNotValidForMorePredicting = bCreateKeyIfNotValidForMorePredicting;
	return MyObj;
}


void UDensAT_WaitTargetDataUsingActor::Activate()
{
	if (!IsValid(this))
	{
		return;
	}

	if (Ability && TargetActor)
	{
		InitializeTargetActor();
		RegisterTargetDataCallbacks();
		FinalizeTargetActor();
	}
	else
	{
		EndTask();
	}
}

void UDensAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	check(AbilitySystemComponent.IsValid());

	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	/** FROM
	 *  Call into the TargetActor to verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if(TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
		
	}
	else
	{
		{if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
		}

		if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
		{
			EndTask();
		}
	}
}


void UDensAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback()
{
	check(AbilitySystemComponent.IsValid());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

void UDensAT_WaitTargetDataUsingActor::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{

	check(AbilitySystemComponent.IsValid());
	if (!Ability)
	{
		return;
	}


	FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent.Get(),ShouldReplicateDataToServer() && (bCreateKeyIfNotValidForMorePredicting && !AbilitySystemComponent->ScopedPredictionKey.IsValidForMorePrediction()));


	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			FGameplayTag ApplicationTag; // TODO: where would this be useful?
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}

	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
	
}




void UDensAT_WaitTargetDataUsingActor::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.IsValid());

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());

	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
	
}



void UDensAT_WaitTargetDataUsingActor::ExternalConfirm(bool bEndTask)
{
	check(AbilitySystemComponent.IsValid());
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}
	Super::ExternalConfirm(bEndTask);
}


void UDensAT_WaitTargetDataUsingActor::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	Super::ExternalCancel();
}




void UDensAT_WaitTargetDataUsingActor::InitializeTargetActor() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->PrimaryPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// If we spawned the target actor, always register the callbacks for when the data is ready.
	TargetActor->TargetDataReadyDelegate.AddUObject(const_cast<UDensAT_WaitTargetDataUsingActor*>(this), &UDensAT_WaitTargetDataUsingActor::OnTargetDataReadyCallback);
	TargetActor->CanceledDelegate.AddUObject(const_cast<UDensAT_WaitTargetDataUsingActor*>(this), &UDensAT_WaitTargetDataUsingActor::OnTargetDataCancelledCallback);
}

void UDensAT_WaitTargetDataUsingActor::FinalizeTargetActor() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->StartTargeting(Ability);

	if (TargetActor->ShouldProduceTargetData())
	{
		// If instant confirm, then stop targeting immediately.
		// Note this is kind of bad: we should be able to just call a static func on the CDO to do this. 
		// But then we wouldn't get to set ExposeOnSpawnParameters.
		if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
		{
			TargetActor->ConfirmTargeting();
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
			TargetActor->BindToConfirmCancelInputs();
		}
	}
}




void UDensAT_WaitTargetDataUsingActor::RegisterTargetDataCallbacks()
{
	if (!IsValid(this))
	{
		return;
	}

	check(Ability);

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UDensAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UDensAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback);

			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}

void UDensAT_WaitTargetDataUsingActor::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		ADensGATargetActor_WeaponTrace* TraceTargetActor = Cast<ADensGATargetActor_WeaponTrace>(TargetActor);
		if (TraceTargetActor)
		{
			TraceTargetActor->StopTargeting();
		}
		else
		{
			// TargetActor doesn't have a StopTargeting function
			TargetActor->SetActorTickEnabled(false);

			// Clear added callbacks
			TargetActor->TargetDataReadyDelegate.RemoveAll(this);
			TargetActor->CanceledDelegate.RemoveAll(this);

			AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
			TargetActor->GenericDelegateBoundASC = nullptr;
		}
	}

	Super::OnDestroy(AbilityEnded);
}

bool UDensAT_WaitTargetDataUsingActor::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Send TargetData to the server if we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}






