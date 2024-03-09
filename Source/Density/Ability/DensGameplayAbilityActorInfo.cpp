// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensGameplayAbilityActorInfo.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"

void FDensGameplayAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UAbilitySystemComponent* InAbilitySystemComponent)
{

	Super::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);
	
	
	//If first person character
	if (AActor* const AvatarActorPtr = AvatarActor.Get())
	{
		ADensCharacter* DensCharacter = Cast<ADensCharacter>(AvatarActorPtr);
		// Grab Components that we care about
		if (DensCharacter)
		{
			SkeletalMeshComponent = DensCharacter->GetFirstPersonMesh();
		}
		
	
		//SkeletalMeshComponent = AvatarActorPtr->GetComopone


	}

	UE_LOG(LogTemp, Warning, TEXT("ACTOR INFO INITIALIZED FROM CUSTOM GAMEPLAYABILITYACTORINFOCLASS"));
}


