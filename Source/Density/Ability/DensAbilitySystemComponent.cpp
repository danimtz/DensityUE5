// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "DensAbilityLibrary.h"
#include "DensAbilityTypes.h"
#include "DensGameplayAbility.h"
#include "DensGameplayTags.h"
#include "GameplayCueManager.h"
#include "AttributeSets/DensBaseStatsAttributeSet.h"
#include "AttributeSets/DensCombatAttributeSet.h"
#include "Characters/DensCharacterBase.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"
#include "..\Characters\PlayerCharacters\DensCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/PhysicalMaterialWithTags.h"
#include "Player/DensPlayerState.h"
#include "Weapons/DensWeaponData.h"
#include "Weapons/DensWeaponInstance.h"
#include "Net/Core/PushModel/PushModel.h"

static TAutoConsoleVariable<float> CVarReplayMontageErrorThreshold(
	TEXT("GS.replay.MontageErrorThreshold"),
	0.5f,
	TEXT("Tolerance level for when montage playback position correction occurs in replays")
);


static TAutoConsoleVariable<bool> CVarGasFixClientSideMontageBlendOutTime(TEXT("AbilitySystem.Fix.ClientSideMontageBlendOutTime"), true, TEXT("Enable a fix to replicate the Montage BlendOutTime for (recently) stopped Montages"));



bool FGameplayAbilityRepAnimMontageForMesh::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Mesh;
	
	RepMontageInfo.NetSerialize(Ar, Map, bOutSuccess);

	
	bOutSuccess = true;
	return true;
	
}







UDensAbilitySystemComponent::UDensAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UDensAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UDensAbilitySystemComponent, RepAnimMontageInfoForMeshes, Params);
	
}

bool UDensAbilitySystemComponent::GetShouldTick() const
{
	
	for (FGameplayAbilityRepAnimMontageForMesh RepMontageInfo : RepAnimMontageInfoForMeshes)
	{
		const bool bHasReplicatedMontageInfoToUpdate = (IsOwnerActorAuthoritative() && RepMontageInfo.RepMontageInfo.IsStopped == false);

		if (bHasReplicatedMontageInfoToUpdate)
		{
			return true;
		}
	}


	
	return Super::GetShouldTick();
}

void UDensAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsOwnerActorAuthoritative())
	{
		for (FGameplayAbilityLocalAnimMontageForMesh& MontageInfo : LocalAnimMontageInfoForMeshes)
		{
			AnimMontage_UpdateReplicatedDataForMesh(MontageInfo.Mesh);
		}
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDensAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
	bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
	
	// AnimatingAbility ended?, clear the pointer
	ClearAnimatingAbilityForAllMeshes(Ability);
}


UDensAbilitySystemComponent* UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UDensAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

FGameplayAbilitySpecHandle UDensAbilitySystemComponent::FindAbilitySpecHandleForClass( TSubclassOf<UGameplayAbility> AbilityClass)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		TSubclassOf<UGameplayAbility> SpecAbilityClass = Spec.Ability->GetClass();
		if (SpecAbilityClass == AbilityClass)
		{
			return Spec.Handle;
		}
	}

	return FGameplayAbilitySpecHandle();
}


bool UDensAbilitySystemComponent::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle,
                                                             bool EndAbilityImmediately)
{
	bool AbilityActivated = false;
	if (InAbilityHandle.IsValid())
	{
		FScopedServerAbilityRPCBatcher DensAbilityRPCBatcher(this, InAbilityHandle);
		AbilityActivated = TryActivateAbility(InAbilityHandle, true);

		if (EndAbilityImmediately)
		{
			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(InAbilityHandle);
			if (AbilitySpec)
			{
				UDensGameplayAbility* DensGameplayAbility = Cast<UDensGameplayAbility>(AbilitySpec->GetPrimaryInstance());
				DensGameplayAbility->ExternalEndAbility();
			}
		}

		return AbilityActivated;
	}

	return AbilityActivated;
}



void UDensAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}



void UDensAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);

}



void UDensAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);

}





void UDensAbilitySystemComponent::ExecuteGameplayCueLocalOnOwningClient_Implementation(
	const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	ExecuteGameplayCueLocal(GameplayCueTag, GameplayCueParameters);
}



float UDensAbilitySystemComponent::PlayMontageForMesh(UGameplayAbility* InAnimatingAbility, USkeletalMeshComponent* InMesh,
                                                      FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, bool bReplicateMontage)
{
	UDensGameplayAbility* InAbility = Cast<UDensGameplayAbility>(InAnimatingAbility);

	float Duration = -1.f;

	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	if(AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate);
		if(Duration > 0.f)
		{
			FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);

			if (AnimMontageInfo.LocalMontageInfo.AnimatingAbility.IsValid() && AnimMontageInfo.LocalMontageInfo.AnimatingAbility != InAnimatingAbility)
			{
				// The ability that was previously animating will have already gotten the 'interrupted' callback.
				// It may be a good idea to make this a global policy and 'cancel' the ability.
				// 
				// For now, we expect it to end itself when this happens.
			}

			if (NewAnimMontage->HasRootMotion() && AnimInstance->GetOwningActor())
			{
				UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
					, *GetNameSafe(NewAnimMontage)
					, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
			}
			
			AnimMontageInfo.LocalMontageInfo.AnimMontage = NewAnimMontage;
			AnimMontageInfo.LocalMontageInfo.AnimatingAbility = InAnimatingAbility;


			if (InAbility)
			{
				InAbility->SetCurrentMontageForMesh(InMesh, NewAnimMontage);
			}
			
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, NewAnimMontage);
			}
			
			// Replicate to non owners
			if (IsOwnerActorAuthoritative())
			{
				if (bReplicateMontage)
				{
					// Those are static parameters, they are only set when the montage is played. They are not changed after that.
					FGameplayAbilityRepAnimMontageForMesh& AbilityRepMontageInfo = GetGameplayAbilityRepAnimMontageForMesh(InMesh);
					AbilityRepMontageInfo.RepMontageInfo.AnimMontage = NewAnimMontage;

					// Update parameters that change during Montage life time.
					AnimMontage_UpdateReplicatedDataForMesh(InMesh);

					// Force net update on our avatar actor
					if (AbilityActorInfo->AvatarActor != nullptr)
					{
						AbilityActorInfo->AvatarActor->ForceNetUpdate();
					}
				}
			}
			else
			{
				// If this prediction key is rejected, we need to end the preview
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				if (PredictionKey.IsValidKey())
				{
					PredictionKey.NewRejectedDelegate().BindUObject(this, &UDensAbilitySystemComponent::OnPredictiveMontageRejectedForMesh, InMesh, NewAnimMontage);
				}
			}
		}
	}

	
	return Duration;
}

float UDensAbilitySystemComponent::PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* NewAnimMontage,
	float InPlayRate, FName StartSectionName)
{
	float Duration = -1.f;
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor */? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate);
		if (Duration > 0.f)
		{
			FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
			AnimMontageInfo.LocalMontageInfo.AnimMontage = NewAnimMontage;
		}
	}

	return Duration;
}

void UDensAbilitySystemComponent::CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	UAnimInstance* AnimInstance = IsValid(InMesh)/* && InMesh->GetOwner() == AbilityActorInfo->AvatarActor */? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
	UAnimMontage* MontageToStop = AnimMontageInfo.LocalMontageInfo.AnimMontage;
	bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if (bShouldStopMontage)
	{
		const float BlendOutTime = (OverrideBlendOutTime >= 0.0f ? OverrideBlendOutTime : MontageToStop->BlendOut.GetBlendTime());

		AnimInstance->Montage_Stop(BlendOutTime, MontageToStop);

		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
	}
}

void UDensAbilitySystemComponent::StopAllCurrentMontages(float OverrideBlendOutTime)
{
	for (FGameplayAbilityLocalAnimMontageForMesh& GameplayAbilityLocalAnimMontageForMesh : LocalAnimMontageInfoForMeshes)
	{
		CurrentMontageStopForMesh(GameplayAbilityLocalAnimMontageForMesh.Mesh, OverrideBlendOutTime);
	}
}

void UDensAbilitySystemComponent::StopMontageIfCurrentForMesh(USkeletalMeshComponent* InMesh,
	const UAnimMontage& Montage, float OverrideBlendOutTime)
{
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
	if (&Montage == AnimMontageInfo.LocalMontageInfo.AnimMontage)
	{
		CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
	}
}

void UDensAbilitySystemComponent::ClearAnimatingAbilityForAllMeshes(UGameplayAbility* Ability)
{
	UDensGameplayAbility* DensAbility = Cast<UDensGameplayAbility>(Ability);
	for (FGameplayAbilityLocalAnimMontageForMesh& GameplayAbilityLocalAnimMontageForMesh : LocalAnimMontageInfoForMeshes)
	{
		if (GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimatingAbility == Ability)
		{
			DensAbility->SetCurrentMontageForMesh(GameplayAbilityLocalAnimMontageForMesh.Mesh, nullptr);
			GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimatingAbility = nullptr;
		}
	}
}

void UDensAbilitySystemComponent::CurrentMontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor */? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
	if ((SectionName != NAME_None) && AnimInstance && AnimMontageInfo.LocalMontageInfo.AnimMontage)
	{
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageInfo.LocalMontageInfo.AnimMontage);
		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
		else
		{
			ServerCurrentMontageJumpToSectionNameForMesh(InMesh, AnimMontageInfo.LocalMontageInfo.AnimMontage, SectionName);
		}
	}
}

void UDensAbilitySystemComponent::CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh,
	FName FromSectionName, FName ToSectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
	if (AnimMontageInfo.LocalMontageInfo.AnimMontage && AnimInstance)
	{
		// Set Next Section Name. 
		AnimInstance->Montage_SetNextSection(FromSectionName, ToSectionName, AnimMontageInfo.LocalMontageInfo.AnimMontage);

		// Update replicated version for Simulated Proxies if we are on the server.
		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
		else
		{
			float CurrentPosition = AnimInstance->Montage_GetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage);
			ServerCurrentMontageSetNextSectionNameForMesh(InMesh, AnimMontageInfo.LocalMontageInfo.AnimMontage, CurrentPosition, FromSectionName, ToSectionName);
		}
	}
}

void UDensAbilitySystemComponent::CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, float InPlayRate)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
	if (AnimMontageInfo.LocalMontageInfo.AnimMontage && AnimInstance)
	{
		// Set Play Rate
		AnimInstance->Montage_SetPlayRate(AnimMontageInfo.LocalMontageInfo.AnimMontage, InPlayRate);

		// Update replicated version for Simulated Proxies if we are on the server.
		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
		else
		{
			ServerCurrentMontageSetPlayRateForMesh(InMesh, AnimMontageInfo.LocalMontageInfo.AnimMontage, InPlayRate);
		}
	}
}

bool UDensAbilitySystemComponent::IsAnimatingAbilityForAnyMesh(UGameplayAbility* InAbility) const
{
	for (FGameplayAbilityLocalAnimMontageForMesh GameplayAbilityLocalAnimMontageForMesh : LocalAnimMontageInfoForMeshes)
	{
		if (GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimatingAbility == InAbility)
		{
			return true;
		}
	}

	return false;
}

UGameplayAbility* UDensAbilitySystemComponent::GetAnimatingAbilityFromAnyMesh()
{
	// Only one ability can be animating for all meshes
	for (FGameplayAbilityLocalAnimMontageForMesh& GameplayAbilityLocalAnimMontageForMesh : LocalAnimMontageInfoForMeshes)
	{
		if (GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimatingAbility.IsValid())
		{
			return GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimatingAbility.Get();
		}
	}

	return nullptr;
}

TArray<UAnimMontage*> UDensAbilitySystemComponent::GetCurrentMontages() const
{
	TArray<UAnimMontage*> Montages;

	for (FGameplayAbilityLocalAnimMontageForMesh GameplayAbilityLocalAnimMontageForMesh : LocalAnimMontageInfoForMeshes)
	{
		UAnimInstance* AnimInstance = IsValid(GameplayAbilityLocalAnimMontageForMesh.Mesh) 
			/*&& GameplayAbilityLocalAnimMontageForMesh.Mesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? GameplayAbilityLocalAnimMontageForMesh.Mesh->GetAnimInstance() : nullptr;

		if (GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimMontage && AnimInstance 
			&& AnimInstance->Montage_IsActive(GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimMontage))
		{
			Montages.Add(GameplayAbilityLocalAnimMontageForMesh.LocalMontageInfo.AnimMontage);
		}
	}

	return Montages;
}

UAnimMontage* UDensAbilitySystemComponent::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor */? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);

	if (AnimMontageInfo.LocalMontageInfo.AnimMontage && AnimInstance
		&& AnimInstance->Montage_IsActive(AnimMontageInfo.LocalMontageInfo.AnimMontage))
	{
		return AnimMontageInfo.LocalMontageInfo.AnimMontage;
	}

	return nullptr;
}

int32 UDensAbilitySystemComponent::GetCurrentMontageSectionIDForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);

	if (CurrentAnimMontage && AnimInstance)
	{
		float MontagePosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		return CurrentAnimMontage->GetSectionIndexFromPosition(MontagePosition);
	}

	return INDEX_NONE;
}

FName UDensAbilitySystemComponent::GetCurrentMontageSectionNameForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);

	if (CurrentAnimMontage && AnimInstance)
	{
		float MontagePosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		int32 CurrentSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(MontagePosition);

		return CurrentAnimMontage->GetSectionName(CurrentSectionID);
	}

	return NAME_None;
}

float UDensAbilitySystemComponent::GetCurrentMontageSectionLengthForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh)/* && InMesh->GetOwner() == AbilityActorInfo->AvatarActor */? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);

	if (CurrentAnimMontage && AnimInstance)
	{
		int32 CurrentSectionID = GetCurrentMontageSectionIDForMesh(InMesh);
		if (CurrentSectionID != INDEX_NONE)
		{
			TArray<FCompositeSection>& CompositeSections = CurrentAnimMontage->CompositeSections;

			// If we have another section after us, then take delta between both start times.
			if (CurrentSectionID < (CompositeSections.Num() - 1))
			{
				return (CompositeSections[CurrentSectionID + 1].GetTime() - CompositeSections[CurrentSectionID].GetTime());
			}
			// Otherwise we are the last section, so take delta with Montage total time.
			else
			{
				return (CurrentAnimMontage->GetPlayLength() - CompositeSections[CurrentSectionID].GetTime());
			}
		}

		// if we have no sections, just return total length of Montage.
		return CurrentAnimMontage->GetPlayLength();
	}

	return 0.f;
}


float UDensAbilitySystemComponent::GetCurrentMontageSectionTimeLeftForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);

	if (CurrentAnimMontage && AnimInstance && AnimInstance->Montage_IsActive(CurrentAnimMontage))
	{
		const float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		return CurrentAnimMontage->GetSectionTimeLeftFromPos(CurrentPosition);
	}

	return -1.f;
}



void UDensAbilitySystemComponent::RemoveMeshFromAnimMontageInfoForMeshes(USkeletalMeshComponent* InMesh)
{

	int i = 0;
	for (FGameplayAbilityLocalAnimMontageForMesh& MontageInfo : LocalAnimMontageInfoForMeshes)
	{
		if (MontageInfo.Mesh == InMesh)
		{
			LocalAnimMontageInfoForMeshes.RemoveAtSwap(i);
			break;
		}
		i++;
	}
	
	
	i = 0;
	for (FGameplayAbilityRepAnimMontageForMesh& RepMontageInfo : RepAnimMontageInfoForMeshes)
	{
		if (RepMontageInfo.Mesh == InMesh)
		{
			RepAnimMontageInfoForMeshes.RemoveAtSwap(i);
			break;
		}
		i++;
	}
}




// ******************************************************************************************************************************************** //
// ******************************************************************************************************************************************** //
// **	 Damage Pipeline								                                                                                   **   //
// ******************************************************************************************************************************************** //
// ******************************************************************************************************************************************** //
void UDensAbilitySystemComponent::ApplyDamageContainerSpec(const FDensDamageContainerSpec& ContainerSpec)
{
	
	//Are we on the server? If not, we should not be applying and calculating damage!
	if(!IsOwnerActorAuthoritative())
	{
		return;
	}
	
	if(ADensCharacterBase* OwningActor = Cast<ADensCharacterBase>(GetAvatarActor()))
	{
		
		
		/* Execute damage pipeline */
		
		//First reset all outgoing meta attributes for buffs Outgoing damage gets overriden each time. OutgoingEmpowerBuff and OutgoingSurgeBuff
		UDensCombatComponent* OwnerCombatComponent = UDensCombatComponent::FindCombatComponent(OwningActor);
		OwnerCombatComponent->SetOutgoingEmpowerBuff(1.0f);
		OwnerCombatComponent->SetOutgoingSurgeBuff(1.0f);
		
		// Set OutgoingDamage Attribute applying GameplayEffect on instigator
		check(ContainerSpec.BaseDamageEffectSpec.Data.IsValid());
		ApplyGameplayEffectSpecToSelf(*ContainerSpec.BaseDamageEffectSpec.Data.Get());
		
		// Trigger ModifyOutgoingDamage gameplay event
		FGameplayEventData ModifyDamageEventDataPayload;
		ModifyDamageEventDataPayload.Instigator = OwningActor;
		ModifyDamageEventDataPayload.ContextHandle = ContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext();
		ModifyDamageEventDataPayload.InstigatorTags = *ContainerSpec.DamageApplyEffectSpec.Data->CapturedSourceTags.GetAggregatedTags();
		HandleGameplayEvent(DensGameplayTags::Event_ModifyOutgoingDamage, &ModifyDamageEventDataPayload);

		// For each target
		for( auto& TargetDataData : ContainerSpec.TargetData.Data)
		{
			FGameplayAbilityTargetData_SingleTargetHit* TargetData_Single = static_cast<FGameplayAbilityTargetData_SingleTargetHit*>(TargetDataData.Get());
			AActor* TargetActor = TargetData_Single->GetHitResult()->GetActor();


			//Create TargetDataHandle just for the single hit result
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			FHitResult HitResult = TargetData_Single->HitResult;
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
			TargetDataHandle.Add(NewTargetData);
			
			
			//Check so that enemies from same team wont damage each other
			{
				const IEnemyFactionInterface* MainActor = Cast<IEnemyFactionInterface>(OwningActor);
				const IEnemyFactionInterface* EnemyActor = Cast<IEnemyFactionInterface>(TargetActor); 
				if(MainActor && EnemyActor)
				{
					FGameplayTagContainer EnemyFactions = MainActor->GetEnemyFactions();
					FGameplayTag OtherCharFaction = EnemyActor->GetOwnFaction();

					//If not enemies, go to next iteration of loop
					if(!EnemyFactions.HasTag(OtherCharFaction))
					{
						continue;
					}
				}
			}

			
			//TODO Add a section here to deal with damage falloff


			
			UDensCombatComponent* TargetCombatComponent = UDensCombatComponent::FindCombatComponent(TargetActor);
			UDensAbilitySystemComponent* TargetDensASC = UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(TargetActor);
			if(TargetCombatComponent && TargetDensASC)
			{
				//Add HitResult to EffectContext
				ContainerSpec.DamageApplyEffectSpec.Data->GetContext().Get()->AddHitResult(HitResult, true);

				//Test if the ability can and should crit and update context
				bool bIsCrit = false;
				FDensGameplayEffectContext* Context = static_cast<FDensGameplayEffectContext*>(ContainerSpec.DamageApplyEffectSpec.Data->GetContext().Get());
				Context->SetIsCriticalHit(false);
				if(ContainerSpec.bDamageCanCrit)
				{
					if (const UPhysicalMaterialWithTags* PhysMat_Target = Cast<UPhysicalMaterialWithTags>(TargetData_Single->GetHitResult()->PhysMaterial.Get()))
					{
						if (PhysMat_Target->Tags.HasTag(DensGameplayTags::Gameplay_Zone_Weakspot))
						{
							bIsCrit = true;
							Context->SetIsCriticalHit(bIsCrit);
							
						}

						//TODO Call event on enemy ASC with gameplay tags
					}
				}

				
				// Set Target IncomingDamage Attribute to current instigator OutgoingDamage and multiply the damage buckets TODO Do this witha gameplay effect perhaps. Potential data race?
				const float CurrOutgoingDamage = OwnerCombatComponent->GetOutgoingDamage();
				const float CurrEmpower = OwnerCombatComponent->GetOutgoingEmpowerBuff();
				const float CurrSurge = OwnerCombatComponent->GetOutgoingSurgeBuff();
				float OutDamage = CurrOutgoingDamage*CurrEmpower*CurrSurge;
				
				float OutStaggerDamage = OwnerCombatComponent->GetOutgoingStaggerDamage(); //TODO add stagger damamge modifiers eg high caliber bullets
				
					
				//If damage type is a weapon and it can bIsCrit is true (and is a player since AI combatants cant crit the player)
				if( ADensCharacter* OwnerDensCharacter = Cast<ADensCharacter>(OwningActor))
				{
					if(Context->GetDamageTypeTags().HasTag(DensGameplayTags::DamgeType_Weapon))
					{
						if(bIsCrit)
						{
							float WeaponCritModifier = OwnerDensCharacter->GetWeaponEquipmentComponent()->ActiveWeaponInstance->GetWeaponCritModifier();
							OutDamage = OutDamage * WeaponCritModifier;
							OutStaggerDamage = OutStaggerDamage * WeaponCritModifier;
						}

						float Distance = FVector::Dist(HitResult.ImpactPoint, HitResult.TraceStart);

						float DistanceAttenuation = 1.0f;
						IDensDamageSource* DamageSource = ContainerSpec.GetDamageSource();
						if(DamageSource)
						{
							DistanceAttenuation = DamageSource->GetDistanceAttenuation(Distance);
						}
						OutDamage = OutDamage * DistanceAttenuation;
					}
				}

				//TODO Do damage falloff mod here as well, add it to weapon instance
				
				
				TargetCombatComponent->SetIncomingDamage(OutDamage);
				TargetCombatComponent->SetIncomingStaggerDamage(OutStaggerDamage);
				
				//Reset meta attribute for incoming buffs: 	IncomingWeakenDebuff and IncomingDamageResistBuff
				TargetCombatComponent->SetIncomingWeakenDebuff(1.0f);
				TargetCombatComponent->SetIncomingDamageResistBuff(1.0f);

				
				// Call BeforeDamage event (eg, apply weaken effects)
				FGameplayEventData EventDataPayload;
				EventDataPayload.EventTag = DensGameplayTags::Event_BeforeDamage;
				EventDataPayload.Instigator = OwningActor;
				EventDataPayload.Target = TargetActor;
				EventDataPayload.ContextHandle = ContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext();
				EventDataPayload.InstigatorTags = *ContainerSpec.DamageApplyEffectSpec.Data->CapturedSourceTags.GetAggregatedTags();
				EventDataPayload.TargetTags = *ContainerSpec.DamageApplyEffectSpec.Data->CapturedTargetTags.GetAggregatedTags();
				EventDataPayload.TargetData = TargetDataHandle;//TODO CREATE NEW HANDLE TargetData_Single;
				
				HandleGameplayEvent(EventDataPayload.EventTag, &EventDataPayload);


				//Check if target has immune tag
				if(TargetDensASC->HasMatchingGameplayTag(DensGameplayTags::State_Combat_Immune))
				{
					TargetCombatComponent->SetIncomingDamage(0.0f); //Set damage to 0
					TargetCombatComponent->SetIncomingStaggerDamage(0.0f);
				}
					
				// Call Modify Incoming Damage event on the target (eg, resilience and other damage resists)
				EventDataPayload.EventTag = DensGameplayTags::Event_ModifyIncomingDamage;
				TargetDensASC->HandleGameplayEvent(EventDataPayload.EventTag, &EventDataPayload);


				// Weaken effects are dealt with in the damage MMC
				
				// Apply Damage Apply GameplayEffect on target
				check(ContainerSpec.DamageApplyEffectSpec.Data.IsValid());
				
				UDensAbilityLibrary::SetTargetDataInEffectContext(ContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext(), TargetDataHandle);
				
				FActiveGameplayEffectHandle EffectHandle = ApplyGameplayEffectSpecToTarget(*ContainerSpec.DamageApplyEffectSpec.Data.Get(), TargetDensASC);


				//TODO apply extra gameplay effects(can include time based gameplay effects for movement speed and other non damage/health related values)
				for( auto ExtraEffect : ContainerSpec.ExtraEffectSpecs)
				{
					ApplyGameplayEffectSpecToTarget(*ExtraEffect.Data.Get(), TargetDensASC);
				}
				
				/*OnGameplayEffectAppliedDelegateToTarget;
				//OnGameplayEffectAppliedToSelf();
				//Damage Numbers gameplay cue??
				{
					FGameplayCueParameters CueParams;

					CueParams.RawMagnitude = 10.0f;

					//ExecuteGameplayCue(DensGameplayTags::GameplayCue_Character_DamageReact, ContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext());
				*/
				

				
				
				//Call On-Hit Events
				EventDataPayload.EventTag = DensGameplayTags::Event_OnDamageHit;
				HandleGameplayEvent(EventDataPayload.EventTag, &EventDataPayload);


				//Damage hit mark cue
				{
					ExecuteGameplayCueLocalOnOwningClient(DensGameplayTags::GameplayCue_UI_HitMark, ContainerSpec.DamageApplyEffectSpec.Data->GetEffectContext());
				}
				
				
				//Event OnDeath called from characterbase
				
			}

		}
	
	}
	
}

void UDensAbilitySystemComponent::SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 NewLevel)
{
	
	//Are we on the server? If not, we should not be modifying Ability Specs!
	if(!IsOwnerActorAuthoritative())
	{
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromClass(AbilityClass);

	if(AbilitySpec)
	{
		AbilitySpec->Level = NewLevel;
		MarkAbilitySpecDirty(*AbilitySpec);
	}
	
	
}


void UDensAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}




void UDensAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}


void UDensAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	/*if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}*/


	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops FROM LYRA

	//@TODO: Add ability activation policies?
	
	// Process all abilities that activate when the input is held.
	/*for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const ULyraGameplayAbility* LyraAbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec->Ability);

				if (LyraAbilityCDO->GetActivationPolicy() == ELyraAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}*/
	

	//@TODO: For now all abilities trigger when input pressed that frame
	
	// Process all abilities that had their input pressed this frame. 
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					//const ULyraGameplayAbility* LyraAbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec->Ability);
					//if (LyraAbilityCDO->GetActivationPolicy() == ELyraAbilityActivationPolicy::OnInputTriggered)
					//{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					//}
				}
			}
		}
	}
	

	/* 
	* Try to activate all the abilities that are from presses and holds.
	* We do it all at once so that held inputs don't activate the ability
	* and then also send a input event to the ability because of the press.
	*/
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}


void UDensAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}




void UDensAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDensAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}


void UDensAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	LocalAnimMontageInfoForMeshes = TArray<FGameplayAbilityLocalAnimMontageForMesh>();
	RepAnimMontageInfoForMeshes = TArray<FGameplayAbilityRepAnimMontageForMesh>();

	if (bPendingMontageRep)
	{
		OnRep_ReplicatedAnimMontageForMesh();
	}
	
	if(InAvatarActor != nullptr)
	{
		ABILITYLIST_SCOPE_LOCK();
		for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			const UDensGameplayAbility* DensGameplayAbilityCDO = CastChecked<UDensGameplayAbility>(AbilitySpec.Ability);
			if(DensGameplayAbilityCDO->GetActivationPolicy() == EDensAbilityActivationPolicy::OnSpawn)
			{
				DensGameplayAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
			}
		}
	}
	
}


void UDensAbilitySystemComponent::RemoveGameplayCue_Internal(const FGameplayTag GameplayCueTag, FActiveGameplayCueContainer& GameplayCueContainer)
{
	if (IsOwnerActorAuthoritative())
	{
		bool bWasInList = HasMatchingGameplayTag(GameplayCueTag);

		// Force replication so GameplayCue removals are properly replicated to all clients during Mixed and Minimal replication modes
		ForceReplication();
		if (bWasInList)
		{
			// Hold onto the cue params before removing our cue
			TArray<const FGameplayCueParameters*> CueParams;
			for (const FActiveGameplayCue& GameplayCue : GameplayCueContainer.GameplayCues)
			{
				if (GameplayCue.GameplayCueTag == GameplayCueTag)
				{
					CueParams.Add(&GameplayCue.Parameters);
				}
			}
            
			GameplayCueContainer.RemoveCue(GameplayCueTag); // Remove the cue

			// Invoke removal event
			for (const FGameplayCueParameters* CueParam : CueParams)
			{
				// Call on server here, clients get it from repnotify
				InvokeGameplayCueEvent(GameplayCueTag, EGameplayCueEvent::Removed, *CueParam);
			}
		}
		// Don't need to multicast broadcast this, ActiveGameplayCues replication handles it
	}
	else if (ScopedPredictionKey.IsLocalClientKey())
	{
		GameplayCueContainer.PredictiveRemove(GameplayCueTag);
	}
}

FGameplayAbilityLocalAnimMontageForMesh& UDensAbilitySystemComponent::GetLocalAnimMontageInfoForMesh(
	USkeletalMeshComponent* InMesh)
{
	for (FGameplayAbilityLocalAnimMontageForMesh& MontageInfo : LocalAnimMontageInfoForMeshes)
	{
		if (MontageInfo.Mesh == InMesh)
		{
			return MontageInfo;
		}
	}

	FGameplayAbilityLocalAnimMontageForMesh MontageInfo = FGameplayAbilityLocalAnimMontageForMesh(InMesh);
	LocalAnimMontageInfoForMeshes.Add(MontageInfo);
	return LocalAnimMontageInfoForMeshes.Last();
}

FGameplayAbilityRepAnimMontageForMesh& UDensAbilitySystemComponent::GetGameplayAbilityRepAnimMontageForMesh(
	USkeletalMeshComponent* InMesh)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(UDensAbilitySystemComponent, RepAnimMontageInfoForMeshes, this);
	for (FGameplayAbilityRepAnimMontageForMesh& RepMontageInfo : RepAnimMontageInfoForMeshes)
	{
		if (RepMontageInfo.Mesh == InMesh)
		{
			return RepMontageInfo;
		}
	}

	FGameplayAbilityRepAnimMontageForMesh RepMontageInfo = FGameplayAbilityRepAnimMontageForMesh(InMesh);
	RepAnimMontageInfoForMeshes.Add(RepMontageInfo);
	return RepAnimMontageInfoForMeshes.Last();
}

void UDensAbilitySystemComponent::OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage)
{
	static const float MONTAGE_PREDICTION_REJECT_FADETIME = 0.25f;

	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance && PredictiveMontage)
	{
		// If this montage is still playing: kill it
		if (AnimInstance->Montage_IsPlaying(PredictiveMontage))
		{
			AnimInstance->Montage_Stop(MONTAGE_PREDICTION_REJECT_FADETIME, PredictiveMontage);
		}
	}
}


void UDensAbilitySystemComponent::AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh)
{
	check(IsOwnerActorAuthoritative());

	AnimMontage_UpdateReplicatedDataForMesh(GetGameplayAbilityRepAnimMontageForMesh(InMesh));
}


void UDensAbilitySystemComponent::AnimMontage_UpdateReplicatedDataForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo)
{
	UAnimInstance* AnimInstance = IsValid(OutRepAnimMontageInfo.Mesh) /*&& OutRepAnimMontageInfo.Mesh->GetOwner() 
		== AbilityActorInfo->AvatarActor */? OutRepAnimMontageInfo.Mesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(OutRepAnimMontageInfo.Mesh);

	if (AnimInstance && AnimMontageInfo.LocalMontageInfo.AnimMontage)
	{
		OutRepAnimMontageInfo.RepMontageInfo.AnimMontage = AnimMontageInfo.LocalMontageInfo.AnimMontage;

		// Compressed Flags
		bool bIsStopped = AnimInstance->Montage_GetIsStopped(AnimMontageInfo.LocalMontageInfo.AnimMontage);

		if (!bIsStopped)
		{
			OutRepAnimMontageInfo.RepMontageInfo.PlayRate = AnimInstance->Montage_GetPlayRate(AnimMontageInfo.LocalMontageInfo.AnimMontage);
			OutRepAnimMontageInfo.RepMontageInfo.Position = AnimInstance->Montage_GetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage);
			OutRepAnimMontageInfo.RepMontageInfo.BlendTime = AnimInstance->Montage_GetBlendTime(AnimMontageInfo.LocalMontageInfo.AnimMontage);
		}

		if (OutRepAnimMontageInfo.RepMontageInfo.IsStopped != bIsStopped)
		{
			// Set this prior to calling UpdateShouldTick, so we start ticking if we are playing a Montage
			OutRepAnimMontageInfo.RepMontageInfo.IsStopped = bIsStopped;


			if (bIsStopped) //NOTE TO FUTURE DANI: Adding this from the normal ASC fixed the blend out time being 0
			{
				// Use AnyThread because GetValueOnGameThread will fail check() when doing replays
				constexpr bool bForceGameThreadValue = true;
				if (CVarGasFixClientSideMontageBlendOutTime.GetValueOnAnyThread(bForceGameThreadValue))
				{
					// Replicate blend out time. This requires a manual search since Montage_GetBlendTime will fail
					// in GetActiveInstanceForMontage for Montages that are stopped.
					for (const FAnimMontageInstance* MontageInstance : AnimInstance->MontageInstances)
					{
						if (MontageInstance->Montage == AnimMontageInfo.LocalMontageInfo.AnimMontage)
						{
							OutRepAnimMontageInfo.RepMontageInfo.BlendTime = MontageInstance->GetBlendTime();
							break;
						}
					}
				}
			}
			
			// When we start or stop an animation, update the clients right away for the Avatar Actor
			if (AbilityActorInfo->AvatarActor != nullptr)
			{
				AbilityActorInfo->AvatarActor->ForceNetUpdate();
			}

			// When this changes, we should update whether or not we should be ticking
			UpdateShouldTick();
		}

		// Replicate NextSectionID to keep it in sync.
		// We actually replicate NextSectionID+1 on a BYTE to put INDEX_NONE in there.
		int32 CurrentSectionID = AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionIndexFromPosition(OutRepAnimMontageInfo.RepMontageInfo.Position);
		if (CurrentSectionID != INDEX_NONE)
		{
			int32 NextSectionID = AnimInstance->Montage_GetNextSectionID(AnimMontageInfo.LocalMontageInfo.AnimMontage, CurrentSectionID);
			if (NextSectionID >= (256 - 1))
			{
				ABILITY_LOG(Error, TEXT("AnimMontage_UpdateReplicatedData. NextSectionID = %d.  RepAnimMontageInfo.Position: %.2f, CurrentSectionID: %d. LocalAnimMontageInfo.AnimMontage %s"),
					NextSectionID, OutRepAnimMontageInfo.RepMontageInfo.Position, CurrentSectionID, *GetNameSafe(AnimMontageInfo.LocalMontageInfo.AnimMontage));
				ensure(NextSectionID < (256 - 1));
			}
			OutRepAnimMontageInfo.RepMontageInfo.NextSectionID = uint8(NextSectionID + 1);
		}
		else
		{
			OutRepAnimMontageInfo.RepMontageInfo.NextSectionID = 0;
		}
	}
}

void UDensAbilitySystemComponent::AnimMontage_UpdateForcedPlayFlagsForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo)
{
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(OutRepAnimMontageInfo.Mesh);
}

void UDensAbilitySystemComponent::OnRep_ReplicatedAnimMontageForMesh()
{
	for (FGameplayAbilityRepAnimMontageForMesh& NewRepMontageInfoForMesh : RepAnimMontageInfoForMeshes)
	{
		FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(NewRepMontageInfoForMesh.Mesh);

		UWorld* World = GetWorld();

		if (NewRepMontageInfoForMesh.RepMontageInfo.bSkipPlayRate)
		{
			NewRepMontageInfoForMesh.RepMontageInfo.PlayRate = 1.f;
		}

		const bool bIsPlayingReplay = World && World->IsPlayingReplay();

		const float MONTAGE_REP_POS_ERR_THRESH = bIsPlayingReplay ? CVarReplayMontageErrorThreshold.GetValueOnGameThread() : 0.1f;

		UAnimInstance* AnimInstance = IsValid(NewRepMontageInfoForMesh.Mesh)/* && NewRepMontageInfoForMesh.Mesh->GetOwner()
			== AbilityActorInfo->AvatarActor*/ ? NewRepMontageInfoForMesh.Mesh->GetAnimInstance() : nullptr;
		if (AnimInstance == nullptr || !IsReadyForReplicatedMontageForMesh())
		{
			// We can't handle this yet
			bPendingMontageRep = true;

			UE_LOG(LogTemp, Error, TEXT("Cannot replicate montage in OnRep_ReplicatedAnimMonrageForMesh. Mesh not valid [%s]. Montage to play was: [%s]"),  *GetNameSafe(this), *GetNameSafe(NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage));
			return;
		}
		bPendingMontageRep = false;

		if (!AbilityActorInfo->IsLocallyControlled())
		{
			static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("net.Montage.Debug"));
			bool DebugMontage = (CVar && CVar->GetValueOnGameThread() == 1);
			if (DebugMontage)
			{
				ABILITY_LOG(Warning, TEXT("\n\nOnRep_ReplicatedAnimMontage, %s"), *GetNameSafe(this));
				ABILITY_LOG(Warning, TEXT("\tAnimMontage: %s\n\tPlayRate: %f\n\tPosition: %f\n\tBlendTime: %f\n\tNextSectionID: %d\n\tIsStopped: %d"),
					*GetNameSafe(NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage),
					NewRepMontageInfoForMesh.RepMontageInfo.PlayRate,
					NewRepMontageInfoForMesh.RepMontageInfo.Position,
					NewRepMontageInfoForMesh.RepMontageInfo.BlendTime,
					NewRepMontageInfoForMesh.RepMontageInfo.NextSectionID,
					NewRepMontageInfoForMesh.RepMontageInfo.IsStopped);
				ABILITY_LOG(Warning, TEXT("\tLocalAnimMontageInfo.AnimMontage: %s\n\tPosition: %f"),
					*GetNameSafe(AnimMontageInfo.LocalMontageInfo.AnimMontage), AnimInstance->Montage_GetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage));
			}

			if (NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage)
			{
				// New Montage to play
				if ((AnimMontageInfo.LocalMontageInfo.AnimMontage != NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage))
				{
					PlayMontageSimulatedForMesh(NewRepMontageInfoForMesh.Mesh, NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage, NewRepMontageInfoForMesh.RepMontageInfo.PlayRate);
				}

				if (AnimMontageInfo.LocalMontageInfo.AnimMontage == nullptr)
				{
					ABILITY_LOG(Warning, TEXT("OnRep_ReplicatedAnimMontage: PlayMontageSimulated failed. Name: %s, AnimMontage: %s"), *GetNameSafe(this), *GetNameSafe(NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage));
					return;
				}

				// Play Rate has changed
				if (AnimInstance->Montage_GetPlayRate(AnimMontageInfo.LocalMontageInfo.AnimMontage) != NewRepMontageInfoForMesh.RepMontageInfo.PlayRate)
				{
					AnimInstance->Montage_SetPlayRate(AnimMontageInfo.LocalMontageInfo.AnimMontage, NewRepMontageInfoForMesh.RepMontageInfo.PlayRate);
				}

				// Compressed Flags
				const bool bIsStopped = AnimInstance->Montage_GetIsStopped(AnimMontageInfo.LocalMontageInfo.AnimMontage);
				const bool bReplicatedIsStopped = bool(NewRepMontageInfoForMesh.RepMontageInfo.IsStopped);

				// Process stopping first, so we don't change sections and cause blending to pop.
				if (bReplicatedIsStopped)
				{
					if (!bIsStopped)
					{
						CurrentMontageStopForMesh(NewRepMontageInfoForMesh.Mesh, NewRepMontageInfoForMesh.RepMontageInfo.BlendTime);

					}
					
					//Reset saved AnimMontage, this fixes replication of same ability montage in a row 
					AnimMontageInfo.LocalMontageInfo.AnimMontage = nullptr;
				}
				else if (!NewRepMontageInfoForMesh.RepMontageInfo.SkipPositionCorrection)
				{
					const int32 RepSectionID = AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionIndexFromPosition(NewRepMontageInfoForMesh.RepMontageInfo.Position);
					const int32 RepNextSectionID = int32(NewRepMontageInfoForMesh.RepMontageInfo.NextSectionID) - 1;

					// And NextSectionID for the replicated SectionID.
					if (RepSectionID != INDEX_NONE)
					{
						const int32 NextSectionID = AnimInstance->Montage_GetNextSectionID(AnimMontageInfo.LocalMontageInfo.AnimMontage, RepSectionID);

						// If NextSectionID is different than the replicated one, then set it.
						if (NextSectionID != RepNextSectionID)
						{
							AnimInstance->Montage_SetNextSection(AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionName(RepSectionID), AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionName(RepNextSectionID), AnimMontageInfo.LocalMontageInfo.AnimMontage);
						}

						// Make sure we haven't received that update too late and the client hasn't already jumped to another section. 
						const int32 CurrentSectionID = AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionIndexFromPosition(AnimInstance->Montage_GetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage));
						if ((CurrentSectionID != RepSectionID) && (CurrentSectionID != RepNextSectionID))
						{
							// Client is in a wrong section, teleport him into the begining of the right section
							const float SectionStartTime = AnimMontageInfo.LocalMontageInfo.AnimMontage->GetAnimCompositeSection(RepSectionID).GetTime();
							AnimInstance->Montage_SetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage, SectionStartTime);
						}
					}

					// Update Position. If error is too great, jump to replicated position.
					const float CurrentPosition = AnimInstance->Montage_GetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage);
					const int32 CurrentSectionID = AnimMontageInfo.LocalMontageInfo.AnimMontage->GetSectionIndexFromPosition(CurrentPosition);
					const float DeltaPosition = NewRepMontageInfoForMesh.RepMontageInfo.Position - CurrentPosition;

					// Only check threshold if we are located in the same section. Different sections require a bit more work as we could be jumping around the timeline.
					// And therefore DeltaPosition is not as trivial to determine.
					if ((CurrentSectionID == RepSectionID) && (FMath::Abs(DeltaPosition) > MONTAGE_REP_POS_ERR_THRESH) && (NewRepMontageInfoForMesh.RepMontageInfo.IsStopped == 0))
					{
						// fast forward to server position and trigger notifies
						if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(NewRepMontageInfoForMesh.RepMontageInfo.AnimMontage))
						{
							// Skip triggering notifies if we're going backwards in time, we've already triggered them.
							const float DeltaTime = !FMath::IsNearlyZero(NewRepMontageInfoForMesh.RepMontageInfo.PlayRate) ? (DeltaPosition / NewRepMontageInfoForMesh.RepMontageInfo.PlayRate) : 0.f;
							if (DeltaTime >= 0.f)
							{
								MontageInstance->UpdateWeight(DeltaTime);
								MontageInstance->HandleEvents(CurrentPosition, NewRepMontageInfoForMesh.RepMontageInfo.Position, nullptr);
								AnimInstance->TriggerAnimNotifies(DeltaTime);
							}
						}
						AnimInstance->Montage_SetPosition(AnimMontageInfo.LocalMontageInfo.AnimMontage, NewRepMontageInfoForMesh.RepMontageInfo.Position);
					}
				}
			}
		}
	}
}

bool UDensAbilitySystemComponent::IsReadyForReplicatedMontageForMesh()
{
	/** Children may want to override this for additional checks (e.g, "has skin been applied") */
	return true;
}


void UDensAbilitySystemComponent::ServerCurrentMontageSetNextSectionNameForMesh_Implementation( USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);

	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = AnimMontageInfo.LocalMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set NextSectionName
			AnimInstance->Montage_SetNextSection(SectionName, NextSectionName, CurrentAnimMontage);

			// Correct position if we are in an invalid section
			float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
			int32 CurrentSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(CurrentPosition);
			FName CurrentSectionName = CurrentAnimMontage->GetSectionName(CurrentSectionID);

			int32 ClientSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(ClientPosition);
			FName ClientCurrentSectionName = CurrentAnimMontage->GetSectionName(ClientSectionID);
			if ((CurrentSectionName != ClientCurrentSectionName) || (CurrentSectionName != SectionName))
			{
				// We are in an invalid section, jump to client's position.
				AnimInstance->Montage_SetPosition(CurrentAnimMontage, ClientPosition);
			}

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UDensAbilitySystemComponent::ServerCurrentMontageSetNextSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName)
{
	return true;
}

void UDensAbilitySystemComponent::ServerCurrentMontageJumpToSectionNameForMesh_Implementation(
	USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);

	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = AnimMontageInfo.LocalMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set NextSectionName
			AnimInstance->Montage_JumpToSection(SectionName, CurrentAnimMontage);

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UDensAbilitySystemComponent::ServerCurrentMontageJumpToSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh,
	UAnimMontage* ClientAnimMontage, FName SectionName)
{
	return true;
}

void UDensAbilitySystemComponent::ServerCurrentMontageSetPlayRateForMesh_Implementation(USkeletalMeshComponent* InMesh,
	UAnimMontage* ClientAnimMontage, float InPlayRate)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) /*&& InMesh->GetOwner() == AbilityActorInfo->AvatarActor*/ ? InMesh->GetAnimInstance() : nullptr;
	FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);

	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = AnimMontageInfo.LocalMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set PlayRate
			AnimInstance->Montage_SetPlayRate(AnimMontageInfo.LocalMontageInfo.AnimMontage, InPlayRate);

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UDensAbilitySystemComponent::ServerCurrentMontageSetPlayRateForMesh_Validate(USkeletalMeshComponent* InMesh,
	UAnimMontage* ClientAnimMontage, float InPlayRate)
{
	return true;
}
