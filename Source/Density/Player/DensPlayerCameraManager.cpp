// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensPlayerCameraManager.h"

#include "DensGameplayTags.h"
#include "DensPlayerState.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"

UCameraShakeBase* ADensPlayerCameraManager::StartCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale,
                                                             ECameraShakePlaySpace PlaySpace, FRotator UserPlaySpaceRot)
{
	APlayerController* PC = GetOwningPlayerController();
	
	ADensCharacter* DensCharacter = Cast<ADensCharacter>(PC->GetCharacter());
	
	if(DensCharacter)
	{
		UDensAbilitySystemComponent* DensASC = DensCharacter->GetDensAbilitySystemComponent();

		float ADSShakeMod =  DensCharacter->GetWeaponEquipmentComponent()->GetActiveWeaponADSShakeModifier();
		
		if(DensASC->HasMatchingGameplayTag(DensGameplayTags::State_Movement_WeaponADS))
		{
			Scale = Scale*ADSShakeMod;
		}
	}

	
	Super::StartCameraShake(ShakeClass, Scale, PlaySpace, UserPlaySpaceRot);

	return nullptr;
}
