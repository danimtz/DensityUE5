// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensWeaponEquipmentComponent.h"

#include "Engine/World.h"

#include "Characters/PlayerCharacters/DensCharacter.h"

#include "Components/SkeletalMeshComponent.h"

#include "Player/DensPlayerState.h"

#include "Ability/AttributeSets/DensBaseWeaponStatsAttributeSet.h"

#include "Weapons/DensWeapon.h"

#include "Weapons/DensWeaponData.h"

#include "Ability/DensAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/DensPlayerController.h"
#include "UserInterface/DensHUDWidget.h"
#include "UserInterface/Reticles/DensCombatReticleWidget.h"
#include "Weapons/DensWeaponInstance.h"


UDensWeaponEquipmentComponent::UDensWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	
	
	ActiveWeaponInstance = nullptr;

	WeaponInstances.Init(nullptr, 3);
	
	

	bWantsInitializeComponent = true;
}

void UDensWeaponEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ADensCharacter* OwnerDensCharacter = GetOwner<ADensCharacter>();
	OwnerDensCharacter->OnFirstThirdPersonSwapDelegate.AddUniqueDynamic(this, &ThisClass::OnFirstToThirdPersonMeshSwap);
	OwnerDensCharacter->OnCharacterDied.AddUniqueDynamic(this, &ThisClass::OnPlayerDeath);
	OwnerDensCharacter->OnAbilitySystemInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemInitialized);



	

}



void UDensWeaponEquipmentComponent::OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC)
{
	
	OwnerASC = InASC;
	

	ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
	if (PS)
	{
		ActiveWeapon_StatsAttributeSet = PS->GetEquippedWeaponAttributeSet();
	}


}

void UDensWeaponEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( UDensWeaponEquipmentComponent, SpawnedWeaponThirdPerson );
}

/*
void UDensWeaponEquipmentComponent::TEMPInitializeCurrentWeapon()
{

	ChangeActiveWeapon(EnergyWeaponInstance);
	
	ActiveWeapon_SavedAttributes->CurrentReserves = EnergyWeaponInstance->TotalReserves;
	ActiveWeapon_SavedAttributes->CurrentMagAmmo = EnergyWeaponInstance->MagazineSize;
	
	EquipWeapon();
}*/





void UDensWeaponEquipmentComponent::InitNewWeapon(TSubclassOf<UDensWeaponInstance> WeaponInstanceClass, bool bAlreadyEquipped)
{
	EWeaponSlot Slot = WeaponInstanceClass.GetDefaultObject()->WeaponSlot;
	
	//If weapon instance is null
	if(!WeaponInstances[static_cast<int>(Slot)])
	{
		//Destroy current weapon instance and unequip it
		//UnEquipWeapon() //TODO call a version that destroys Ability sets of equipped weapons not currently active. For example of passives like Harmony. For now dont call since only currently equipped active weapons have active ability sets

		
		DestroyWeaponInstanceObject(Slot);

		//Spawn new weapon instance
		WeaponInstances[static_cast<int>(Slot)] = SpawnWeaponInstanceObject(WeaponInstanceClass);


		if(!bAlreadyEquipped)
		{
			InitWeaponSavedAttribute(Slot);
		}
			
		WeaponInstances[static_cast<int>(Slot)]->SetCurrentAmmo(GetSavedWeaponAttributes(Slot)->CurrentMagAmmo, GetSavedWeaponAttributes(Slot)->CurrentReserves);
		

		
		ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();
		ADensPlayerController* DensPC = Cast<ADensPlayerController>(OwnerCharacter->GetController());
		if(DensPC)
		{
			UDensHUDWidget* MainCombatHUD = DensPC->GetMainHUD();
			if(MainCombatHUD)
			{
				MainCombatHUD->InitNewWeaponOnHUD(WeaponInstances[static_cast<int>(Slot)]);
			}
		}


	
		//If theres no active weapon set this to active
		if(ActiveWeaponInstance==nullptr)
		{
			ChangeActiveWeapon(WeaponInstances[static_cast<int>(Slot)]);
			EquipWeapon();
		}

		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DensWeaponEquipmentComponent: Init new weapon UDensWeaponInstance is null or not valid for [%s] (Could be fine due to race condition init prevention)"), *GetNameSafe(WeaponInstanceClass));
	}
}

void UDensWeaponEquipmentComponent::UpdateWeaponInstances(float DeltaTime)
{

	for(auto WeaponInstance : WeaponInstances)
	{
		if(WeaponInstance == nullptr)
		{
			continue;
		}
		
		WeaponInstance->UpdateWeaponOnTick(DeltaTime);
	}
	
}


void UDensWeaponEquipmentComponent::EquipKineticWeapon()
{
	
	if (OwnerASC.IsValid())
	{
		UnEquipWeapon();
		ChangeActiveWeapon(WeaponInstances[0]);
		EquipWeapon();
	}
}


void UDensWeaponEquipmentComponent::EquipEnergyWeapon()
{
	if (OwnerASC.IsValid())
	{
		UnEquipWeapon();
		ChangeActiveWeapon(WeaponInstances[1]);
		EquipWeapon();
	}
}

void UDensWeaponEquipmentComponent::EquipHeavyWeapon()
{
	if (OwnerASC.IsValid())
	{
		UnEquipWeapon();
		ChangeActiveWeapon(WeaponInstances[2]);
		EquipWeapon();
	}
}

void UDensWeaponEquipmentComponent::OnAmmoBrickPickedUp_Implementation(EWeaponAmmoType AmmoType, float Value)
{

	if(HasAuthority())
	{
		SaveActiveAttributesToWeaponStats();
		
		int NonFullWeaponTypeCount = 0;
		for(auto WeaponInstance : WeaponInstances)
		{
			if(WeaponInstance->AmmoType == AmmoType)
			{
				FWeaponBasicAttributes* WeaponAttribute = GetSavedWeaponAttributes(WeaponInstance->WeaponSlot);

				if((WeaponAttribute->CurrentReserves + WeaponAttribute->CurrentMagAmmo) < WeaponInstance->TotalReserves)
				{
					NonFullWeaponTypeCount++;
				}
				
			}
		}


		/*if (NonFullWeaponTypeCount == 0)
		{
			return;
		}*/

		float AmmoValuePerWeapon = Value/NonFullWeaponTypeCount;

		
		for(auto WeaponInstance : WeaponInstances)
		{
			if(WeaponInstance->AmmoType == AmmoType)
			{
				FWeaponBasicAttributes* WeaponAttribute = GetSavedWeaponAttributes(WeaponInstance->WeaponSlot);

				if((WeaponAttribute->CurrentReserves + WeaponAttribute->CurrentMagAmmo) < WeaponInstance->TotalReserves)
				{
					float AmmoToRestore = FMath::Floor(AmmoValuePerWeapon * WeaponInstance->AmmoBrickToAmmoRatio);
					float NewReserves = WeaponAttribute->CurrentReserves+AmmoToRestore;

					float MaxReserves = WeaponInstance->TotalReserves - WeaponAttribute->CurrentMagAmmo;
					
					WeaponAttribute->CurrentReserves = FMath::Min(MaxReserves, NewReserves);

					
					
				}
				
			}
		}

		//UpdateHUD if on listen server
		ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();
		ADensPlayerState* DensPS = Cast<ADensPlayerState>(OwnerCharacter->GetPlayerState());
		if(DensPS)
		{
			DensPS->UnEquippedAmmoChanged();
		}
		
		//Update ActiveWeapon ammo values if affected NOTE technically if ammo reserves are full this does nothing
		if(ActiveWeaponInstance->AmmoType == AmmoType)
		{
			FWeaponBasicAttributes* ActiveWeaponAttribute = GetSavedWeaponAttributes(ActiveWeaponInstance->WeaponSlot);
			ActiveWeapon_StatsAttributeSet->SetAmmoReserves(ActiveWeaponAttribute->CurrentReserves);
		}
		

		
	}


	
}


USkeletalMeshComponent* UDensWeaponEquipmentComponent::GetActiveWeaponMeshFirstPerson() const
{
	
	if (SpawnedWeapon)
	{
		ADensWeapon* Weapon = Cast<ADensWeapon>(SpawnedWeapon);

		return Weapon->GetWeaponMesh();
	}

	return nullptr;
}

USkeletalMeshComponent* UDensWeaponEquipmentComponent::GetActiveWeaponMeshThirdPerson() const
{

	if (SpawnedWeaponThirdPerson)
	{
		ADensWeapon* Weapon = Cast<ADensWeapon>(SpawnedWeaponThirdPerson);

		return Weapon->GetWeaponMesh();
	}
	return nullptr;
	
}

ADensWeapon* UDensWeaponEquipmentComponent::GetActiveWeaponFirstPerson() const
{
	if(SpawnedWeapon)
	{
		return SpawnedWeapon;
	}
	return nullptr;
}

ADensWeapon* UDensWeaponEquipmentComponent::GetActiveWeaponThirdPerson() const
{
	if(SpawnedWeaponThirdPerson)
	{
		return SpawnedWeaponThirdPerson;
	}
	return nullptr;
}


UDensWeaponInstance* UDensWeaponEquipmentComponent::GetActiveWeaponInstance() const
{
	return ActiveWeaponInstance;
}


void UDensWeaponEquipmentComponent::OnPlayerDeath(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo)
{
	
	UnEquipWeapon();
	
	OnPlayerDeath_Client();
	
	for(auto WeaponInstance : WeaponInstances)
	{
		WeaponInstance->ConditionalBeginDestroy();
	}
	
}

void UDensWeaponEquipmentComponent::OnPlayerDeath_Client_Implementation()
{
	DestroyWeaponActors();
}


void UDensWeaponEquipmentComponent::OnFirstToThirdPersonMeshSwap(bool bIsFirstPerson)
{
	//TODO make this cleaner, for now it just hides weapon
	const ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();
	if (OwnerCharacter == nullptr)
	{
		return;
	}
	
	USkeletalMeshComponent* WeaponMeshFirstPerson = GetActiveWeaponMeshFirstPerson();
	if( WeaponMeshFirstPerson && OwnerCharacter->IsLocallyControlled())
	{
		WeaponMeshFirstPerson->SetVisibility(bIsFirstPerson);
	}

	USkeletalMeshComponent* WeaponMeshThirdPerson = GetActiveWeaponMeshThirdPerson();
	if( WeaponMeshThirdPerson && OwnerCharacter->IsLocallyControlled())
	{
		WeaponMeshThirdPerson->SetVisibility(!bIsFirstPerson);
	}

	
}




bool UDensWeaponEquipmentComponent::IsAmmoTypeFull(EWeaponAmmoType AmmoType)
{

	
	for(auto WeaponInstance : WeaponInstances)
	{

		if(WeaponInstance->AmmoType == AmmoType)
		{

			if(WeaponInstance == ActiveWeaponInstance)
			{
				if((ActiveWeapon_StatsAttributeSet->GetAmmoReserves() + ActiveWeapon_StatsAttributeSet->GetMagAmmo()) < ActiveWeapon_StatsAttributeSet->GetMaxAmmoReserves())
				{
					return false;
				}
			}
			else
			{
				FWeaponBasicAttributes* WeaponAttribute = GetSavedWeaponAttributes(WeaponInstance->WeaponSlot);

				if((WeaponAttribute->CurrentReserves + WeaponAttribute->CurrentMagAmmo) < WeaponInstance->TotalReserves)
				{
					return false;
				}
			}
		}
	}
	
	return true;
}

float UDensWeaponEquipmentComponent::GetActiveWeaponADSShakeModifier()
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponADSShakeModifier;
	}

	return 1.0f;
}

FWeaponBasicAttributes* UDensWeaponEquipmentComponent::GetSavedWeaponAttributes(EWeaponSlot Slot)
{

	ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();
	
	ADensPlayerState* DensPS = Cast<ADensPlayerState>(OwnerCharacter->GetPlayerState());
	if(DensPS)
	{
		return &DensPS->GetSavedWeaponAttribute(Slot);
	}

	return nullptr;
}


void UDensWeaponEquipmentComponent::ChangeActiveWeapon(UDensWeaponInstance* NewActiveWeapon)
{
	ActiveWeaponInstance = NewActiveWeapon; //change active weapon

	if(HasAuthority())
	{
		FWeaponBasicAttributes* ActiveWeapon_SavedAttributes = GetSavedWeaponAttributes(ActiveWeaponInstance->WeaponSlot);
		ActiveWeapon_SavedAttributes = GetSavedWeaponAttributes(NewActiveWeapon->WeaponSlot);
	}
	
}



void UDensWeaponEquipmentComponent::EquipWeapon()
{

	if (ActiveWeaponInstance)
	{
		ActiveWeaponInstance->SetIsEquipped(true);
		//Setup Weapon Mesh and AnimBP
		ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();

		SpawnWeaponActor(&ActiveWeaponInstance->WeaponData->WeaponToSpawn);

		SpawnWeaponActorThirdPerson(&ActiveWeaponInstance->WeaponData->WeaponToSpawn);
		
		OwnerCharacter->SetupADSWeaponOffset();
		OwnerCharacter->OnWeaponSwapped();

		OnFirstToThirdPersonMeshSwap(OwnerCharacter->IsInFirstPerson()); //Set weapon mesh visibility
		
		

		//Update HUD
		ADensPlayerController* DensPC = Cast<ADensPlayerController>(OwnerCharacter->GetController());
		if(DensPC)
		{
			UDensCombatReticleWidget* ReticleHUD = DensPC->GetReticleHUD();
			if(ReticleHUD)
			{
				ReticleHUD->NewWeaponEquipped(ActiveWeaponInstance);
			}
			
			UDensHUDWidget* MainCombatHUD = DensPC->GetMainHUD();
			if(MainCombatHUD)
			{
				MainCombatHUD->OnNewWeaponEquipped(ActiveWeaponInstance);
			}

			if(HasAuthority()) //Listen server UI change, else it gets called on ONRep
			{
				ADensPlayerState* DensPS = DensPC->GetDensPlayerState();
				if(DensPS)
				{
					DensPS->UnEquippedAmmoChanged();
				}
			
			}
			
			
		}

		//Set Weapon Attributes from Saved Data
		CopyWeaponStatsToActiveAttributeSet();
		
		
		
		//Setup Weapon Abilities
		UDensAbilitySystemComponent* DensASC = OwnerASC.Get();
		if (DensASC)
		{
			for (auto& AbilitySet : ActiveWeaponInstance->WeaponAbilitySets)
			{
				if (AbilitySet)
				{
					AbilitySet->AddToASC(DensASC, &GrantedAbilityHandles);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AbilitySet not found or not set for DensWeapon  TODO ADD REFERENCE TO WHICH CHARACTER"));
				}
			}

		}
	}
	

}

void UDensWeaponEquipmentComponent::UnEquipWeapon()
{
	if (ActiveWeaponInstance)
	{
		ActiveWeaponInstance->SetIsEquipped(false);
		DestroyWeaponActors();


		//Save current ammo from attribute set to saved data
		if(HasAuthority())
		{
			SaveActiveAttributesToWeaponStats();
		}

		

		//Remove Weapon Abilities
		UDensAbilitySystemComponent* DensASC = OwnerASC.Get();
		if (DensASC && HasAuthority())
		{

			/*for (auto& AbilitySet : ActiveWeaponInstance->WeaponAbilitySets)
			{
				if (AbilitySet)
				{
					//Remove effects with tag (used for perks like rampage which should reset when swapping weapons)

					AbilitySet->RemovePerkEffects(DensASC);
					
					AbilitySet->RemoveFromASC(DensASC);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Weapon ability set could not be removed"));
				}
			}*/

			
			GrantedAbilityHandles.RemovePerkEffectsFromASC(DensASC);
			GrantedAbilityHandles.RemoveAbilitiesFromASC(DensASC);
		}

		
	}
	

}

//Copy base values of weapon to Saved Attribute struct
void UDensWeaponEquipmentComponent::InitWeaponSavedAttribute( EWeaponSlot Slot)
{
	UDensWeaponInstance* WeaponInstance = WeaponInstances[static_cast<int>(Slot)];

	if(HasAuthority())
	{
		FWeaponBasicAttributes* WeaponAttribute = GetSavedWeaponAttributes(Slot);
		//WeaponSavedAttributes[static_cast<int>(Slot)].CurrentReserves = WeaponInstance->TotalReserves;
		//WeaponSavedAttributes[static_cast<int>(Slot)].CurrentMagAmmo = WeaponInstance->MagazineSize;
	
		WeaponAttribute->CurrentReserves = WeaponInstance->TotalReserves - WeaponInstance->MagazineSize;
		WeaponAttribute->CurrentMagAmmo = WeaponInstance->MagazineSize;
		WeaponAttribute->AmmoType = WeaponInstance->AmmoType;
	}
	

	
}

void UDensWeaponEquipmentComponent::CopyWeaponStatsToActiveAttributeSet()
{
	if(ActiveWeapon_StatsAttributeSet.IsValid())
	{
		//Base stats
		ActiveWeapon_StatsAttributeSet->SetRoundsPerMinute( float(ActiveWeaponInstance->RoundsPerMinute) );
		ActiveWeapon_StatsAttributeSet->SetMaxMagSize( float(ActiveWeaponInstance->MagazineSize) );
		ActiveWeapon_StatsAttributeSet->SetMaxAmmoReserves(ActiveWeaponInstance->TotalReserves); 
		ActiveWeapon_StatsAttributeSet->SetVerticalRecoil(ActiveWeaponInstance->VerticalRecoilDirection);
		ActiveWeapon_StatsAttributeSet->SetADSMovementModifier(ActiveWeaponInstance->ADSMovementModifier);
		
		FWeaponBasicAttributes* ActiveWeapon_SavedAttributes = GetSavedWeaponAttributes(ActiveWeaponInstance->WeaponSlot);
		//Altered stats (current ammo current mag size etc)
		ActiveWeapon_StatsAttributeSet->SetMagAmmo(ActiveWeapon_SavedAttributes->CurrentMagAmmo);
		ActiveWeapon_StatsAttributeSet->SetAmmoReserves(ActiveWeapon_SavedAttributes->CurrentReserves);
	}
	
}

void UDensWeaponEquipmentComponent::SaveActiveAttributesToWeaponStats()
{
	if(ActiveWeapon_StatsAttributeSet.IsValid())
	{
		FWeaponBasicAttributes* ActiveWeapon_SavedAttributes = GetSavedWeaponAttributes(ActiveWeaponInstance->WeaponSlot);
		ActiveWeapon_SavedAttributes->CurrentReserves = ActiveWeapon_StatsAttributeSet->GetAmmoReserves();
		ActiveWeapon_SavedAttributes->CurrentMagAmmo = ActiveWeapon_StatsAttributeSet->GetMagAmmo();
		ActiveWeapon_SavedAttributes->ReplicationCounter = 	ActiveWeapon_SavedAttributes->ReplicationCounter + 1; 
		//ActiveWeapon_SavedAttributes->AmmoType = ActiveWeaponInstance->AmmoType; //Redundant
	}
}


float UDensWeaponEquipmentComponent::GetFireDelay() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float RPM =  ActiveWeapon_StatsAttributeSet->GetRoundsPerMinute();

		//Convert to FireDelay
		if (RPM != 0)
		{
			float FireDelay = 1.0f / ( RPM / 60.0f );

			return FireDelay;
		}
	}

	return 100.0f;
}

float UDensWeaponEquipmentComponent::GetMagAmmo() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float MagAmmo =  ActiveWeapon_StatsAttributeSet->GetMagAmmo();
		return MagAmmo;
	}

	return 0.0f;
}



float UDensWeaponEquipmentComponent::GetMaxMagSize() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float MaxMagSize =  ActiveWeapon_StatsAttributeSet->GetMaxMagSize();
		return MaxMagSize;
	}

	return 0.0f;
}

float UDensWeaponEquipmentComponent::GetAmmoReserves() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float AmmoReserves =  ActiveWeapon_StatsAttributeSet->GetAmmoReserves();
		return AmmoReserves;
	}

	return 0.0f;
}


float UDensWeaponEquipmentComponent::GetMaxAmmoReserves() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float MaxAmmoReserves =  ActiveWeapon_StatsAttributeSet->GetMaxAmmoReserves();
		return MaxAmmoReserves;
	}

	return 0.0f;
}


float UDensWeaponEquipmentComponent::GetShotRecoilDirection() const
{
	if (ActiveWeapon_StatsAttributeSet.IsValid())
	{
		float VerticalRecoil =  ActiveWeapon_StatsAttributeSet->GetVerticalRecoil();
		return VerticalRecoil;
	}

	return 0.0f;
	
}


FWeaponActionAnimationSet UDensWeaponEquipmentComponent::GetReadyAnimations() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->EquipAnimation;
		
	}

	return FWeaponActionAnimationSet();
}

FWeaponActionAnimationSet UDensWeaponEquipmentComponent::GetStowAnimations() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->StowAnimation;
	}

	return FWeaponActionAnimationSet();
}

FWeaponActionAnimationSet UDensWeaponEquipmentComponent::GetFireAnimations() const
{
	FWeaponActionAnimationSet AnimSet;
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->FireAnimation;
	}

	return FWeaponActionAnimationSet();
}

FWeaponActionAnimationSet UDensWeaponEquipmentComponent::GetReloadAnimations() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->ReloadAnimation;
	}

	return FWeaponActionAnimationSet();
}


FWeaponActionAnimationSet UDensWeaponEquipmentComponent::GetSlideAnimations() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->SlideAnimation;
	}

	return FWeaponActionAnimationSet();
}

FWeaponAnimationData UDensWeaponEquipmentComponent::GetWeaponAnimations() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->AnimationPoses;
	}

	UE_LOG(LogTemp, Error, TEXT("GetWeaponAnimations() on WeaponEquipmentComponent [%s] is not valid."), *GetNameSafe(this));

	//TODO Error check this
	return FWeaponAnimationData();
}

bool UDensWeaponEquipmentComponent::IsWeaponInstanceValid() const
{
	if(ActiveWeaponInstance)
	{
		return true;
	}
	return false;
	
}

FTransform UDensWeaponEquipmentComponent::GetActiveWeaponADSOffset() const
{
	if(ActiveWeaponInstance)
	{
		return ActiveWeaponInstance->WeaponData->WeaponADSOffset;
	}

	return FTransform();
}

EWeaponSlot UDensWeaponEquipmentComponent::GetActiveWeaponSlot() const
{
	if(ActiveWeaponInstance)
	{
		EWeaponSlot WeaponSlot =  ActiveWeaponInstance->WeaponSlot;
		return WeaponSlot;
	}
	//TODO Error check this
	return EWeaponSlot::Kinetic;
	
}


void UDensWeaponEquipmentComponent::SpawnWeaponActor(TSubclassOf<ADensWeapon>* WeaponToSpawn)
{

	ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();

	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner Character not found in Weapon Equipment Component SpawnWeaponActor for [%s] ."), *GetNameSafe(this));
		return;
	}
	
	AController* DensPC = OwnerCharacter->GetController();
	if(!DensPC)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner Character does not have a valid Controller in SpawnWeaponActor [%s] ."), *GetNameSafe(this));
		return;
	}
	
	if (DensPC->IsLocalController())
	{
		
		USkeletalMeshComponent* AttachMeshTarget = OwnerCharacter->GetFirstPersonMesh();
		
		//TSubclassOf<AActor>* ActorToSpawn = Cast<TSubclassOf<AActor>>(WeaponToSpawn);
		//APawn* OwnerPawn = Cast<APawn>(OwnerCharacter);
		USceneComponent* AttachTarget = Cast<USceneComponent>(AttachMeshTarget);


		if (!SpawnedWeapon && WeaponToSpawn->Get())
		{

			SpawnedWeapon = GetWorld()->SpawnActorDeferred<ADensWeapon>(*WeaponToSpawn, FTransform::Identity, OwnerCharacter);
			
			//NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			SpawnedWeapon->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Grip_RSocket")); //TODO UnHARDCODE THIS
			//SpawnedWeapon->SetOwner(OwnerCharacter);
			SpawnedWeapon->SetWeaponPerspective(true);
			//SpawnedWeapon->SetUsesFirstPersonMaterial(true);
			SpawnedWeapon->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Previous spawned weapon was not removed or Missing Weapon actor in WeaponData"));
			check(WeaponToSpawn->Get());
		}
		
	}
}

void UDensWeaponEquipmentComponent::SpawnWeaponActorThirdPerson(TSubclassOf<ADensWeapon>* WeaponToSpawn)
{

	ADensCharacter* OwnerCharacter = GetOwner<ADensCharacter>();
	if (OwnerCharacter && HasAuthority())
	{
		
		USkeletalMeshComponent* AttachMeshTarget = OwnerCharacter->GetThirdPersonArmsMesh();
		USceneComponent* AttachTarget = Cast<USceneComponent>(AttachMeshTarget);


		if (!SpawnedWeaponThirdPerson && WeaponToSpawn->Get())
		{

			SpawnedWeaponThirdPerson = GetWorld()->SpawnActorDeferred<ADensWeapon>(*WeaponToSpawn, FTransform::Identity, OwnerCharacter);
			//NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			SpawnedWeaponThirdPerson->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Grip_RSocket")); //TODO UnHARDCODE THIS
			//SpawnedWeaponThirdPerson->SetOwner(OwnerCharacter);
			SpawnedWeaponThirdPerson->SetWeaponPerspective(false);
			//SpawnedWeaponThirdPerson->SetUsesFirstPersonMaterial(false);
			SpawnedWeaponThirdPerson->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Previous spawned weapon was not removed or Missing Weapon actor in WeaponData"));
			check(WeaponToSpawn->Get());
		}
		
	}



	
}


void UDensWeaponEquipmentComponent::DestroyWeaponActors()
{
	//for (AActor* Actor : SpawnedWeapons)
	//{

	//Remove from ASC montage meshes

	UDensAbilitySystemComponent* DensASC = OwnerASC.Get();
	if(DensASC)
	{
		if(SpawnedWeapon)
		{
			DensASC->RemoveMeshFromAnimMontageInfoForMeshes(SpawnedWeapon->GetWeaponMesh());
		}
		
		if (SpawnedWeaponThirdPerson)
		{
			DensASC->RemoveMeshFromAnimMontageInfoForMeshes(SpawnedWeaponThirdPerson->GetWeaponMesh());
		}
	}

	
	if (SpawnedWeapon)
	{
		SpawnedWeapon->Destroy();
		SpawnedWeapon = nullptr;
	}

	if (SpawnedWeaponThirdPerson)
	{
		SpawnedWeaponThirdPerson->Destroy();
		SpawnedWeaponThirdPerson = nullptr;
	}
	//}
}

void UDensWeaponEquipmentComponent::DestroyWeaponInstanceObject(EWeaponSlot WeaponSlot)
{
	//if nullptr dont destroy
	if(!WeaponInstances[static_cast<uint8>(WeaponSlot)])
	{
		return;
	}
		
	WeaponInstances[static_cast<uint8>(WeaponSlot)]->ConditionalBeginDestroy();
	WeaponInstances[static_cast<uint8>(WeaponSlot)] = nullptr;
}



TObjectPtr<UDensWeaponInstance> UDensWeaponEquipmentComponent::SpawnWeaponInstanceObject(TSubclassOf<UDensWeaponInstance> WeaponInstanceToSpawn)
{
	if (WeaponInstanceToSpawn)
	{
		UDensWeaponInstance* WeaponInstance;

		WeaponInstance = NewObject<UDensWeaponInstance>(this, WeaponInstanceToSpawn);
		//WeaponInstance = GetWorld()->SpawnActorDeferred<UDensWeaponInstance>(*WeaponInstanceToSpawn, FTransform::Identity, GetOwner());
		//WeaponInstance->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
		//UObject* outer = WeaponInstance->GetOuter();
		WeaponInstance->DensCharacterOwner = Cast<ADensCharacter>(GetOuter());
		WeaponInstance->WeaponEquipmentComponentOwner = this;
		//NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
		//SpawnedWeapon->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Grip_RSocket")); //TODO UnHARDCODE THIS

		return WeaponInstance;
	}

	
	return nullptr;
}
