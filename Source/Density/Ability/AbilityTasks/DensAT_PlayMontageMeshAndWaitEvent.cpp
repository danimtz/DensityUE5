// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAT_PlayMontageMeshAndWaitEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Animation/AnimInstance.h"

#include "GameFramework/Character.h"

UDensAT_PlayMontageMeshAndWaitEvent::UDensAT_PlayMontageMeshAndWaitEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UDensAbilitySystemComponent* UDensAT_PlayMontageMeshAndWaitEvent::GetTargetASC()
{
	return Cast<UDensAbilitySystemComponent>(AbilitySystemComponent);
}

void UDensAT_PlayMontageMeshAndWaitEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UDensAT_PlayMontageMeshAndWaitEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage(OverrideBlendOutTimeForCancelAbility))
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UDensAT_PlayMontageMeshAndWaitEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UDensAT_PlayMontageMeshAndWaitEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UDensAT_PlayMontageMeshAndWaitEvent* UDensAT_PlayMontageMeshAndWaitEvent::PlayMontageForMeshAndWaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, USkeletalMeshComponent* InMesh, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection,
	bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, bool bReplicateMontage, float OverrideBlendOutTimeForCancelAbility,
	float OverrideBlendOutTimeForStopWhenEndAbility)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UDensAT_PlayMontageMeshAndWaitEvent* MyObj = NewAbilityTask<UDensAT_PlayMontageMeshAndWaitEvent>(OwningAbility, TaskInstanceName);
	MyObj->Mesh = InMesh;
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->bReplicateMontage = bReplicateMontage;
	MyObj->OverrideBlendOutTimeForCancelAbility = OverrideBlendOutTimeForCancelAbility;
	MyObj->OverrideBlendOutTimeForStopWhenEndAbility = OverrideBlendOutTimeForStopWhenEndAbility;

	return MyObj;
}

void UDensAT_PlayMontageMeshAndWaitEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid Mesh"), *FString(__FUNCTION__));
		return;
	}

	bool bPlayedMontage = false;
	UDensAbilitySystemComponent* DensASC = GetTargetASC();

	if (DensASC)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			// Bind to event callback
			EventHandle = DensASC->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UDensAT_PlayMontageMeshAndWaitEvent::OnGameplayEvent));

			if (DensASC->PlayMontageForMesh(Ability, Mesh, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, bReplicateMontage) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UDensAT_PlayMontageMeshAndWaitEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UDensAT_PlayMontageMeshAndWaitEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UDensAT_PlayMontageMeshAndWaitEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UDensAbilityTask_PlayMontageMeshAndWaitEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDensAbilityTask_PlayMontageMeshAndWaitEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDensAbilityTask_PlayMontageMeshAndWaitEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UDensAT_PlayMontageMeshAndWaitEvent::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UDensAT_PlayMontageMeshAndWaitEvent::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage(OverrideBlendOutTimeForStopWhenEndAbility);
		}
	}

	UDensAbilitySystemComponent* DensASC = GetTargetASC();
	if (DensASC)
	{
		DensASC->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UDensAT_PlayMontageMeshAndWaitEvent::StopPlayingMontage(float OverrideBlendOutTime)
{
	if (Mesh == nullptr)
	{
		return false;
	}

	UDensAbilitySystemComponent* DensASC = GetTargetASC();
	if (!DensASC)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	if (DensASC && Ability)
	{
		if (DensASC->GetAnimatingAbilityFromAnyMesh() == Ability
			&& DensASC->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			DensASC->CurrentMontageStopForMesh(Mesh, OverrideBlendOutTime);
			return true;
		}
	}

	return false;
}

FString UDensAT_PlayMontageMeshAndWaitEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability && Mesh)
	{
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}



