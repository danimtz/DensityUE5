// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeapon.h"
#include "DensWeaponData.h"

#include "Components/SkeletalMeshComponent.h"
#include "Math/TransformNonVectorized.h"

#include "Ability/DensAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Characters/PlayerCharacters/DensCharacter.h"


// Sets default values
ADensWeapon::ADensWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SetReplicates(true);
	bReplicates = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->SetupAttachment(RootComponent);
	RootComponent = WeaponMesh;
	
	bIsFirstPersonWeapon = false;
}

// Called when the game starts or when spawned
void ADensWeapon::BeginPlay()
{
	Super::BeginPlay();


	SetUsesFirstPersonMaterial(bIsFirstPersonWeapon);

	
	///TDOD MESH VISIBILITY
	ADensCharacter* OwnerChar = Cast<ADensCharacter>(GetOwner());
	if(!OwnerChar)
	{
		return;
	}

	
	if(bIsFirstPersonWeapon)
	{
		if(OwnerChar->IsInFirstPerson())
		{
			GetWeaponMesh()->SetVisibility(true);
		}
		else
		{
			GetWeaponMesh()->SetVisibility(false);
		}
	}
	else//if its a third person weapon
	{
		if(OwnerChar->IsLocallyControlled())
		{
			if(OwnerChar->IsInFirstPerson())
			{
				GetWeaponMesh()->SetVisibility(false);
			}
			else
			{
				GetWeaponMesh()->SetVisibility(true);
			}
		}
		else
		{
			GetWeaponMesh()->SetVisibility(true);
		}
	}
	
	
	


	
	
	
}


// Called every frame
void ADensWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void ADensWeapon::SetWeaponADSOffset(const FTransform& NewTransform)
{
	WeaponADSOffset = NewTransform;
}
*/
