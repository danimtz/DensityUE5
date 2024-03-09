// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "NativeGameplayTags.h"

class UGameplayTagsManager;

/**
 * 
 */
/*
class DENSITY_API FDensGameplayTags
{

public:

	static const FDensGameplayTags& Get() { return GameplayTags; };

	static void InitializeNativeTags();

	//static FGameplayTag FindTagByString(FString TagString); //Do i need this?

	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Look_Stick;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Crouch;
	FGameplayTag InputTag_Sprint;

	FGameplayTag Event_ModifyOutgoingDamage;
	FGameplayTag Event_ModifyIncomingDamage;
	FGameplayTag Event_BeforeDamage;
	FGameplayTag Event_OnDamageHit;
	FGameplayTag Event_OnDamageTaken;
	FGameplayTag Event_OnShieldBroken;
	FGameplayTag Event_OnKill;
	FGameplayTag Event_Death;

	FGameplayTag Gameplay;
	FGameplayTag Gameplay_Zone_Weakspot;
	FGameplayTag Gameplay_OutgoingDamage;
	
	
	FGameplayTag GameplayCue_Actor_DamageReact;
	FGameplayTag GameplayCue_Character_DamageReact;
	FGameplayTag GameplayCue_UI_DamageNumbers;
	FGameplayTag GameplayCue_UI_HitMark;
	FGameplayTag GameplayCue_UI_HitMarkDeath;
	
	
	FGameplayTag DamgeType_Weapon;

	FGameplayTag State;
	FGameplayTag State_Effects_Weakened;
	FGameplayTag State_Effects;
	FGameplayTag State_Combat_Immune;
	FGameplayTag State_Movement_WeaponADS;
	FGameplayTag State_Movement_Grounded;
	FGameplayTag State_Movement_Airborne;
	FGameplayTag State_Movement_Sprint;
	FGameplayTag State_Dying;
	//FGameplayTag InputTag_AutoRun; //do i need this?

	//TODO add more native tags. Lyra adds more, some for abilities, some for status such as crouching, death, dying etc

protected:

	//Register tags with the gameplay tags manager
	void AddAllTags(UGameplayTagsManager& Manager);

	//Helper function used by add all tags to register single tag with manager
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComponent);



private:

	static FDensGameplayTags GameplayTags;

};
*/

namespace DensGameplayTags
{

	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Move);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Look_Mouse);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Look_Stick);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Jump);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Crouch);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( InputTag_Sprint);

	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_ModifyOutgoingDamage);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_ModifyIncomingDamage);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_BeforeDamage);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_OnDamageHit);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_OnDamageTaken);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_OnShieldBroken);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_OnKill);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_Death);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Event_OnStaggerThresholdCrossed);

	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Gameplay);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Gameplay_Zone_Weakspot);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Gameplay_OutgoingDamage);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( Gameplay_OutgoingStaggerDamage);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_Character_ShieldBroken);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_Actor_DamageReact);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_Character_DamageReact);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_Character_Death);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_UI_DamageNumbers);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_UI_HitMark);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( GameplayCue_UI_HitMarkDeath);
	
	
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( DamgeType_Weapon);

	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Effects_Weakened);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Effects);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Combat_Immune);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Movement_WeaponADS);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Movement_Grounded);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Movement_Airborne);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Movement_Sprint);
	DENSITY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN( State_Dying);
	
}


