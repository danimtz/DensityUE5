// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "DensAbilitySystemComponent.h"
#include "DensAbilityTypes.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"


UDensGameplayAbility::UDensGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	ActivationPolicy = EDensAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FDensDamageContainerSpec UDensGameplayAbility::MakeDamageContainerSpecFromContainer(const FDensDamageContainer& Container)
{
	FDensDamageContainerSpec ReturnSpec;

	AActor* OwningActor = GetOwningActorFromActorInfo();
	UDensAbilitySystemComponent* OwningASC = UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		//Build gameplay effect specs for both effects
		
		ReturnSpec.BaseDamageEffectSpec = MakeOutgoingGameplayEffectSpec(Container.BaseDamageEffectClass);
		
		ReturnSpec.DamageApplyEffectSpec = MakeOutgoingGameplayEffectSpec(Container.DamageApplyEffectClass);

		for( auto& EffectClass : Container.ExtraEffectClasses)
		{
			ReturnSpec.ExtraEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass));
		}
		
		//Set ability can crit
		ReturnSpec.bDamageCanCrit = Container.bDamageCanCrit;

		ReturnSpec.DamageSource = Container.DamageSource;
		
		//Set the DamageType tags to the DamgeApplyEffect context
		FDensGameplayEffectContext* MutableContext = static_cast<FDensGameplayEffectContext*>(ReturnSpec.DamageApplyEffectSpec.Data->GetContext().Get());
		MutableContext->SetDamageTypeTags(Container.DamageTypeTags);
	}
	return ReturnSpec;
}


void UDensGameplayAbility::ApplyDamageContainerSpec(const FDensDamageContainerSpec& Container)
{
	UDensAbilitySystemComponent* DensASC = UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(GetOwningActorFromActorInfo());
	DensASC->ApplyDamageContainerSpec(Container);
}

UDensAbilitySystemComponent* UDensGameplayAbility::GetDensAbilitySystemComponentFromActorInfo() const
{

	if (!ensure(CurrentActorInfo))
	{
		return nullptr;
	}

	if(UDensAbilitySystemComponent* DensASC =  Cast<UDensAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()))
	{
		return DensASC;
	}
	
	return nullptr;
	
}









void UDensGameplayAbility::ExternalEndAbility()
{
	check(CurrentActorInfo);

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UDensGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately)
 {
	UDensAbilitySystemComponent* DensASC = Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (DensASC)
	{
		return DensASC->BatchRPCTryActivateAbility(InAbilityHandle, EndAbilityImmediately);
	}

	return false;
}

FWeaponActionMeshSet UDensGameplayAbility::GetAnimationMeshes() const
{
	FWeaponActionMeshSet ActionMeshes;

	ADensCharacter* DensCharacter = Cast<ADensCharacter>(GetAvatarActorFromActorInfo());
	if(DensCharacter)
	{
		ActionMeshes.FirstPersonMesh = DensCharacter->GetFirstPersonMesh();
		ActionMeshes.ThirdPersonMesh = DensCharacter->GetMesh();
		ActionMeshes.WeaponMeshFirstPerson = DensCharacter->GetWeaponEquipmentComponent()->GetActiveWeaponMeshFirstPerson();
		ActionMeshes.WeaponMeshThirdPerson = DensCharacter->GetWeaponEquipmentComponent()->GetActiveWeaponMeshThirdPerson();
	}

	return ActionMeshes;
	
}

UAnimMontage* UDensGameplayAbility::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		return AbilityMeshMontage.Montage;
	}

	return nullptr;
}

void UDensGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* InCurrentMontage)
{
	ensure(IsInstantiated());

	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InCurrentMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FAbilityMeshMontage(InMesh, InCurrentMontage));
	}
}


void UDensGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UDensGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	
}


void UDensGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	//FROM LYRA. Try activate ability on spawn. Check for replication
	
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EDensAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
	
}




bool UDensGameplayAbility::FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMeshMontage)
{
	for (FAbilityMeshMontage& MeshMontage : CurrentAbilityMeshMontages)
	{
		if (MeshMontage.Mesh == InMesh)
		{
			InAbilityMeshMontage = MeshMontage;
			return true;
		}
	}

	return false;
}

void UDensGameplayAbility::MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	check(CurrentActorInfo);

	UDensAbilitySystemComponent* const AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageJumpToSectionForMesh(InMesh, SectionName);
	}
}

void UDensGameplayAbility::MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName,
	FName ToSectionName)
{
	check(CurrentActorInfo);

	UDensAbilitySystemComponent* const AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageSetNextSectionNameForMesh(InMesh, FromSectionName, ToSectionName);
	}
}

void UDensGameplayAbility::MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UDensAbilitySystemComponent* const AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent != nullptr)
	{
		// We should only stop the current montage if we are the animating ability
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
		}
	}
}

void UDensGameplayAbility::MontageStopForAllMeshes(float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UDensAbilitySystemComponent* const AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent != nullptr)
	{
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->StopAllCurrentMontages(OverrideBlendOutTime);
		}
	}
}
