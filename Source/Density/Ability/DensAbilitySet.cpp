// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "Ability/DensAbilitySet.h"

#include "Abilities/GameplayAbility.h"

#include "DensAbilitySystemComponent.h"
#include "DensGameplayAbility.h"
#include "DensGA_WeaponPerk.h"




void FDensAbilitySet_GrantedHandles::AddPerkRemoveTag(FGameplayTag PerkTag)
{
	PerkRemoveTags.AddTag(PerkTag);
}

void FDensAbilitySet_GrantedHandles::SaveAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FDensAbilitySet_GrantedHandles::ResetHandles()
{
	AbilitySpecHandles.Reset();
}


void FDensAbilitySet_GrantedHandles::RemovePerkEffectsFromASC(UDensAbilitySystemComponent* DensASC)
{

	if (!DensASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability.
		return;
	}

	
	for (auto PerkTag : PerkRemoveTags)//TODO MAYBE FOR LOOP ISNT NEEDED
	{
		FGameplayTagContainer PerkRemoveEffectTag = FGameplayTagContainer(PerkTag);
		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(PerkRemoveEffectTag);
		DensASC->RemoveActiveEffects(Query, -1);
		
	}
	
}


void FDensAbilitySet_GrantedHandles::RemoveAbilitiesFromASC(UDensAbilitySystemComponent* DensASC)
{
	check(DensASC);

	if (!DensASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability.
		return;
	}


	//const TArray<FGameplayAbilitySpecHandle>& AbilitySpecHandles = GrantedAbilityHandles.GetAbilitySpecHandles();
	for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilitySpecHandles)
	{
		if (AbilityHandle.IsValid())
		{
			DensASC->ClearAbility(AbilityHandle);
		}
	}
	
	ResetHandles();

}

/*
const TArray<FGameplayAbilitySpecHandle>& FDensAbilitySet_GrantedHandles::GetAbilitySpecHandles() const
{
	return AbilitySpecHandles;
}
*/




UDensAbilitySet::UDensAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDensAbilitySet::AddToASC(UDensAbilitySystemComponent* DensASC, FDensAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) 
{
	check(DensASC);

	if (!DensASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FDensAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}
		
		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		//Check if ability is a weapon perk and add Perk remove tag to OutGrantedHandles
		
		if(UDensGA_WeaponPerk* Perk = Cast<UDensGA_WeaponPerk>(AbilityCDO))
		{
			if(Perk->RemoveEffectTag.IsValid())
			{
				OutGrantedHandles->AddPerkRemoveTag(Perk->RemoveEffectTag);
			}
			
		}
		
		

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = DensASC->GiveAbility(AbilitySpec);

		OutGrantedHandles->SaveAbilitySpecHandle(AbilitySpecHandle);
	
	}

	
}


/*
void UDensAbilitySet::RemovePerkEffects(UDensAbilitySystemComponent* DensASC)
{
	for (auto& AbilityInfo : GrantedGameplayAbilities)
	{
		
		//Check if ability is a weapon perk
		UGameplayAbility* AbilityCDO = AbilityInfo.Ability->GetDefaultObject<UGameplayAbility>();

		if(UDensGA_WeaponPerk* Perk = Cast<UDensGA_WeaponPerk>(AbilityCDO))
		{
			//If it has a remove effect tag then remove the perk effect
			if(Perk->RemoveEffectTag.IsValid())
			{
				FGameplayTagContainer PerkRemoveEffectTag = FGameplayTagContainer(Perk->RemoveEffectTag);
				FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(PerkRemoveEffectTag);
				DensASC->RemoveActiveEffects(Query, -1);
			}
		}
		
	}
	
}*/

