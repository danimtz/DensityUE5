// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensPickupItem.h"

#include "Characters/PlayerCharacters/DensCharacter.h"


// Sets default values
ADensPickupItem::ADensPickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
}

void ADensPickupItem::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	if (Other && Other != this)
	{
		PickUpItem(Cast<ADensCharacter>(Other));
	}
	
	
}

// Called when the game starts or when spawned
/*void ADensPickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}*/


bool ADensPickupItem::K2_CanBePickedUp_Implementation(ADensCharacter* TargetCharacter) const
{
	return true;
}





bool ADensPickupItem::CanPickUp(ADensCharacter* TargetCharacter) const
{
	//bIsActive && TestCharacter && TestCharacter->IsAlive() && IsValid(this) && !TestCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags) && K2_CanBePickedUp(TestCharacter);

	if((TargetCharacter->GetLocalRole() == ROLE_AutonomousProxy) || (TargetCharacter->GetLocalRole() ==  ROLE_Authority))
	{
		return K2_CanBePickedUp(TargetCharacter);
	}

	return false;
}



void ADensPickupItem::PickUpItem(ADensCharacter* TargetCharacter)
{
	if(!TargetCharacter)
	{
		return;
	}
	if(CanPickUp(TargetCharacter))
	{
		ApplyItemToCharacter(TargetCharacter);
		
	}
	
}


void ADensPickupItem::ApplyItemToCharacter(ADensCharacter* TargetCharacter)
{

	K2_OnItemPickedUp(TargetCharacter);
	
	Destroy();
}

