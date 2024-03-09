// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.



#include "DensGameplayTags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

//static singleton
/*
FDensGameplayTags FDensGameplayTags::GameplayTags;


void FDensGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager);

	GameplayTagsManager.DoneAddingNativeTags();
}


void FDensGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	AddTag(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
	AddTag(InputTag_Jump, "InputTag.Jump", "Normal Jump input");
	AddTag(InputTag_Crouch, "InputTag.Crouch", "Crouch input");
	AddTag(InputTag_Sprint, "InputTag.Sprint", "Sprint input");
	AddTag(Event_ModifyOutgoingDamage, "Event.ModifyOutgoingDamage", "To trigger event that modifies outgoing damage");
	AddTag(Event_ModifyIncomingDamage, "Event.ModifyIncomingDamage", "To trigger event that modifies incoming damage");
	AddTag(Event_BeforeDamage, "Event.BeforeDamage", "To trigger before damage event in damage pipeline");
	AddTag(Event_OnDamageHit, "Event.OnDamageHit", "To trigger on hit damage event");
	AddTag(Event_OnDamageTaken, "Event.OnDamageTaken", "To trigger on damaged events on the damage taker");
	AddTag(Event_OnShieldBroken, "Event.OnShieldBroken", "To trigger on shield broken events on the damage taker");
	AddTag(Event_OnKill, "Event.OnKill", "To trigger on kill damage");
	AddTag(Event_Death, "Event.Death", "Trigger Death events");

	AddTag(Gameplay_Zone_Weakspot, "Gameplay.Zone.Weakspot", "Tag to check if location in a critspot");
	AddTag(Gameplay_OutgoingDamage, "Gameplay.OutgoingDamage", "Tag to set outgoing ability damage");
	AddTag(Gameplay, "Gameplay", "Root tag for Gameplay effect tags");

	
	AddTag(GameplayCue_Actor_DamageReact, "GameplayCue.Actor.DamageReact", "Tag for gameplay cue to react to damage");
	AddTag(GameplayCue_Character_DamageReact, "GameplayCue.Character.DamageReact", "Tag for gameplay cue to react to damage");
	AddTag(GameplayCue_UI_DamageNumbers, "GameplayCue.UI.DamageNumbers", "Tag to show UI damage numbers GC");
	AddTag(GameplayCue_UI_HitMark, "GameplayCue.UI.HitMark", "Tag to show UI Hit marker GC");
	AddTag(GameplayCue_UI_HitMarkDeath, "GameplayCue.UI.HitMarkDeath", "Tag to show UI Hit marker death GC");
	
	AddTag(DamgeType_Weapon, "DamageType.Weapon", "DamageType for weapons");

	AddTag(State, "State", "Parent tag for state");
	AddTag(State_Effects, "State.Effects", "Parent tag for effects");
	AddTag(State_Effects_Weakened, "State.Effects.Weakened", "Tag when character is weakened");
	AddTag(State_Combat_Immune, "State.Combat.Immune", "Tag when character is immune to damage (0 damage dealt)");

	AddTag(State_Movement_WeaponADS, "State.Movement.WeaponADS", "Tag when character ADS with weapon");
	AddTag(State_Movement_Airborne, "State.Movement.Airborne", "Tag when character on the airborne");
	AddTag(State_Movement_Grounded, "State.Movement.Grounded", "Tag when character on the ground");
	AddTag(State_Movement_Sprint, "State.Movement.Sprint", "Tag when character is sprinting");
	
	AddTag(State_Dying, "State.Dying", "Tag when character has been killed");

	
	
}



void FDensGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
*/


namespace DensGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Jump, "InputTag.Jump", "Normal Jump input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "Crouch input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Sprint, "InputTag.Sprint", "Sprint input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_ModifyOutgoingDamage, "Event.ModifyOutgoingDamage", "To trigger event that modifies outgoing damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_ModifyIncomingDamage, "Event.ModifyIncomingDamage", "To trigger event that modifies incoming damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_BeforeDamage, "Event.BeforeDamage", "To trigger before damage event in damage pipeline");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_OnDamageHit, "Event.OnDamageHit", "To trigger on hit damage event");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_OnDamageTaken, "Event.OnDamageTaken", "To trigger on damaged events on the damage taker");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_OnShieldBroken, "Event.OnShieldBroken", "To trigger on shield broken events on the damage taker");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_OnKill, "Event.OnKill", "To trigger on kill damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death, "Event.Death", "Trigger Death events");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_OnStaggerThresholdCrossed, "Event.OnStaggerThresholdCrossed", "Trigger Stagger treshold events on damage taker");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Zone_Weakspot, "Gameplay.Zone.Weakspot", "Tag to check if location in a critspot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_OutgoingDamage, "Gameplay.OutgoingDamage", "Tag to set outgoing ability damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_OutgoingStaggerDamage, "Gameplay.OutgoingStaggerDamage", "Tag to set outgoing ability stagger damage");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay, "Gameplay", "Root tag for Gameplay effect tags");

	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Actor_DamageReact, "GameplayCue.Actor.DamageReact", "Tag for gameplay cue to react to damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Character_DamageReact, "GameplayCue.Character.DamageReact", "Tag for gameplay cue to react to damage");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Character_ShieldBroken, "GameplayCue.Character.ShieldBroken", "Tag for gameplay cue to react to character shield breaking");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Character_Death, "GameplayCue.Character.Death", "Tag for gameplay cue to react to death");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_UI_DamageNumbers, "GameplayCue.UI.DamageNumbers", "Tag to show UI damage numbers GC");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_UI_HitMark, "GameplayCue.UI.HitMark", "Tag to show UI Hit marker GC");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_UI_HitMarkDeath, "GameplayCue.UI.HitMarkDeath", "Tag to show UI Hit marker death GC");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamgeType_Weapon, "DamageType.Weapon", "DamageType for weapons");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State, "State", "Parent tag for state");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Effects, "State.Effects", "Parent tag for effects");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Effects_Weakened, "State.Effects.Weakened", "Tag when character is weakened");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Combat_Immune, "State.Combat.Immune", "Tag when character is immune to damage (0 damage dealt)");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Movement_WeaponADS, "State.Movement.WeaponADS", "Tag when character ADS with weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Movement_Airborne, "State.Movement.Airborne", "Tag when character on the airborne");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Movement_Grounded, "State.Movement.Grounded", "Tag when character on the ground");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Movement_Sprint, "State.Movement.Sprint", "Tag when character is sprinting");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dying, "State.Dying", "Tag when character has been killed");
	
}