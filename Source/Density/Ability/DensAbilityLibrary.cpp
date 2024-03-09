// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensAbilityLibrary.h"

#include "DensAbilitySystemComponent.h"
#include "DensAbilityTypes.h"
#include "DensGameplayAbility.h"
#include "DensGameplayTags.h"
#include "Characters/DensCharacterBase.h"
#include "PhysicsEngine/PhysicsAsset.h"

bool UDensAbilityLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContext)
{
	const FDensGameplayEffectContext* DensEffectContext = static_cast<const FDensGameplayEffectContext*>(EffectContext.Get());
	if (DensEffectContext)
	{
		return DensEffectContext->IsCriticalHit();
	}
	return false;
}

void UDensAbilityLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContext, bool bIsCriticalHit)
{
	FDensGameplayEffectContext* DensEffectContext = static_cast<FDensGameplayEffectContext*>(EffectContext.Get());
	if (DensEffectContext)
	{
		DensEffectContext->SetIsCriticalHit(bIsCriticalHit);
	}
}


bool UDensAbilityLibrary::IsWeakened(const FGameplayEffectContextHandle& EffectContext)
{
	const FDensGameplayEffectContext* DensEffectContext = static_cast<const FDensGameplayEffectContext*>(EffectContext.Get());
	if (DensEffectContext)
	{
		return DensEffectContext->IsWeakened();
	}
	return false;
}

void UDensAbilityLibrary::SetIsWeakened(FGameplayEffectContextHandle& EffectContext, bool bIsWeakened)
{
	FDensGameplayEffectContext* DensEffectContext = static_cast<FDensGameplayEffectContext*>(EffectContext.Get());
	if (DensEffectContext)
	{
		DensEffectContext->SetIsWeakened(bIsWeakened);
	}
}



FGameplayAbilityTargetDataHandle UDensAbilityLibrary::GetTargetData(FGameplayEffectContextHandle EffectContextHandle)
{
	FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(EffectContextHandle.Get());
	
	if (EffectContext)
	{
		return EffectContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}



void UDensAbilityLibrary::AddTargetDataToEffectContext(FGameplayEffectContextHandle EffectContextHandle,
	const FGameplayAbilityTargetDataHandle& TargetData)
{
	FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(EffectContextHandle.Get());

	if (EffectContext)
	{
		EffectContext->AddTargetData(TargetData);
	}
}


void UDensAbilityLibrary::SetTargetDataInEffectContext(FGameplayEffectContextHandle EffectContextHandle,
	const FGameplayAbilityTargetDataHandle& TargetData)
{
	FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(EffectContextHandle.Get());

	if (EffectContext)
	{
		EffectContext->SetTargetData(TargetData);
	}
}


void UDensAbilityLibrary::CreateTargetDataFromHitResults(const TArray<FHitResult>& HitResults, FGameplayAbilityTargetDataHandle& TargetData)
{

	FGameplayAbilityTargetDataHandle NewTargetData;
	
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		NewTargetData.Add(NewData);
	}

	TargetData = NewTargetData;
}



FGameplayTagContainer UDensAbilityLibrary::GetDamageTypeTags(const FGameplayEffectContextHandle& EffectContext)
{
	const FDensGameplayEffectContext* DensEffectContext = static_cast<const FDensGameplayEffectContext*>(EffectContext.Get());
	if (DensEffectContext)
	{
		return DensEffectContext->GetDamageTypeTags();
	}
	return FGameplayTagContainer::EmptyContainer;
}

FDensDamageContainerSpec& UDensAbilityLibrary::AddTargetsToDamageContainerSpec(FDensDamageContainerSpec& ContainerSpec,
	const TArray<FHitResult>& HitResults)
{
	ContainerSpec.AddTargets(HitResults);

	return ContainerSpec;
}

FDensDamageContainerSpec& UDensAbilityLibrary::ReplaceTargetsToDamageContainerSpec(
	FDensDamageContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults)
{
	ContainerSpec.ReplaceTargets(HitResults);

	return ContainerSpec;
}

FDensDamageContainerSpec& UDensAbilityLibrary::SetAbilityDamageInDamageContainer(
	 FDensDamageContainerSpec& DamageContainerSpec,  float AbilityDamage)
{
	
	DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingDamage, AbilityDamage);

	return DamageContainerSpec;
}

FDensDamageContainerSpec& UDensAbilityLibrary::SetAbilityStaggerDamageInDamageContainer(
	FDensDamageContainerSpec& DamageContainerSpec, float AbilityStaggerDamage)
{
	DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingStaggerDamage, AbilityStaggerDamage);
	return DamageContainerSpec;
}

FDensDamageContainerSpec& UDensAbilityLibrary::SetAbilityDamageAndStaggerInDamageContainer(
	FDensDamageContainerSpec& DamageContainerSpec, float AbilityDamage, float AbilityStaggerDamage)
{
	DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingDamage, AbilityDamage);
	DamageContainerSpec.BaseDamageEffectSpec.Data->SetSetByCallerMagnitude(DensGameplayTags::Gameplay_OutgoingStaggerDamage, AbilityStaggerDamage);
	
	return DamageContainerSpec;
}


FDensDamageContainerSpec& UDensAbilityLibrary::SetDamageSourceInDamageContainer(
	FDensDamageContainerSpec& DamageContainerSpec, UObject* DamageSource)
{
	DamageContainerSpec.DamageSource = DamageSource;

	return DamageContainerSpec;
}


void UDensAbilityLibrary::ApplyExternalDamageContainer(AActor* Instigator, const FDensDamageContainerSpec& DamageContainerSpec)
{
	
	ADensCharacterBase* InstigatorCharacter = Cast<ADensCharacterBase>(Instigator);
	if(!InstigatorCharacter)
	{
		return;
	}
	
	UDensAbilitySystemComponent* DensASC = InstigatorCharacter->GetDensAbilitySystemComponent();
	if(DensASC)
	{
		DensASC->ApplyDamageContainerSpec(DamageContainerSpec);
	}
}

UDensGameplayAbility* UDensAbilityLibrary::GetPrimaryAbilityInstanceFromHandle(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UDensGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UDensGameplayAbility* UDensAbilityLibrary::GetPrimaryAbilityInstanceFromClass(
	UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass);
		if (AbilitySpec)
		{
			return Cast<UDensGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

bool UDensAbilityLibrary::IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent,
	FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UDensGameplayAbility>(AbilitySpec->GetPrimaryInstance())->IsActive();
		}
	}

	return false;
}

bool UDensAbilityLibrary::IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle)
{
	return Handle.IsValid();
}

bool UDensAbilityLibrary::CheckIsEnemy(const AActor* Actor, const AActor* OtherActor)
{
	
	const IEnemyFactionInterface* MainActor = Cast<IEnemyFactionInterface>(Actor);
	const IEnemyFactionInterface* EnemyActor = Cast<IEnemyFactionInterface>(OtherActor); 

	if(MainActor && EnemyActor)
	{
		
		FGameplayTagContainer EnemyFactions = MainActor->GetEnemyFactions();

		FGameplayTag OtherCharFaction = EnemyActor->GetOwnFaction();
	
		return EnemyFactions.HasTag(OtherCharFaction);
	}

	
	return false;
}


/*
static void RecursiveFindChildBone(USkeletalMeshComponent* SkeletalMesh, int32 ParentIndex, ECollisionEnabled::Type CollisionType)
{

	TArray<int32> ChildrenIndex;
	SkeletalMesh->GetSkeletalMeshAsset()->GetSkeleton()->GetChildBones(ParentIndex, ChildrenIndex);

	for(auto Index : ChildrenIndex)
	{

		RecursiveFindChildBone(SkeletalMesh, Index, CollisionType);

			
		FName ChildrenBoneName = SkeletalMesh->GetBoneName(Index);
		UE_LOG(LogTemp, Warning, TEXT("Bone: %s"), *ChildrenBoneName.ToString());
		if(FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(ChildrenBoneName))
		{
			BodyInstance->SetShapeCollisionEnabled(0, CollisionType);
		}
	}
	
}
*/

void UDensAbilityLibrary::ChangeCollisionOnPhysicsBody(USkeletalMeshComponent* SkeletalMesh, FName BoneName,
	ECollisionEnabled::Type CollisionType, bool bAffectChildren)
{

	
	if(FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(BoneName))
	{
		BodyInstance->SetShapeCollisionEnabled(0, CollisionType);
	}

	/*
	if(bAffectChildren)
	{

		
		int ParentIndex = SkeletalMesh->GetBoneIndex(BoneName);
		TArray<int32> ChildrenIndex;
		SkeletalMesh->GetSkeletalMeshAsset()->GetSkeleton()->GetChildBones(ParentIndex, ChildrenIndex);

		UE_LOG(LogTemp, Warning, TEXT("Bone: %s"), *BoneName.ToString());

		for(auto Index : ChildrenIndex)
		{

			RecursiveFindChildBone(SkeletalMesh, Index, CollisionType);

			
			FName ChildrenBoneName = SkeletalMesh->GetBoneName(Index);
			UE_LOG(LogTemp, Warning, TEXT("Bone: %s"), *ChildrenBoneName.ToString());
			if(FBodyInstance* BodyInstance = SkeletalMesh->GetBodyInstance(ChildrenBoneName))
			{
				BodyInstance->SetShapeCollisionEnabled(0, CollisionType);
			}
		}
	}*/
}

