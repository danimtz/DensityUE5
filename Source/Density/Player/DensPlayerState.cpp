// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensPlayerState.h"

#include "DensGameplayTags.h"
#include "DensPlayerController.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/AttributeSets/DensCoreStatsAttributeSet.h"
#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"
#include "Ability/AttributeSets/DensBaseWeaponStatsAttributeSet.h"
#include "Ability/AttributeSets/DensAbilityEnergyAttributeSet.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"


#include "Components/GameFrameworkComponentManager.h"
#include "Components/PlayerStateComponent.h"
#include "Inventory/DensEquipmentManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "UserInterface/DensHUDWidget.h"
#include "UserInterface/Reticles/DensCombatReticleWidget.h"

ADensPlayerState::ADensPlayerState()
{

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UDensAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us. From GASDocumentation
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 50.0f;
	
	//Init attribute sets
	CoreStatsAttributeSet = CreateDefaultSubobject<UDensCoreStatsAttributeSet>(TEXT("CoreStatsAttributeSet"));
	BaseStatsAttributeSet = CreateDefaultSubobject<UDensBaseStatsAttributeSet>(TEXT("BaseStatsAttributeSet"));
	AbilityEnergyAttributeSet = CreateDefaultSubobject<UDensAbilityEnergyAttributeSet>(TEXT("AbilityEnergyAttributeSet"));
	
	CombatAttributeSet = CreateDefaultSubobject<UDensCombatAttributeSet>(TEXT("CombatAttributeSet"));
	
	EquippedWeaponAttributeSet = CreateDefaultSubobject<UDensBaseWeaponStatsAttributeSet>(TEXT("EquippedKineticWeaponAttributeSet"));
	
	EquipmentManagerComponent = CreateDefaultSubobject<UDensEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));

	WeaponSavedAttributes.Init(FWeaponBasicAttributes(), 3);
}


UAbilitySystemComponent* ADensPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UDensEquipmentManagerComponent* ADensPlayerState::GetEquipmentManagerComponent() const
{
	return EquipmentManagerComponent;
}

UDensCoreStatsAttributeSet* ADensPlayerState::GetCoreStatsAttributeSet() const
{
	return CoreStatsAttributeSet;
}

UDensBaseStatsAttributeSet* ADensPlayerState::GetBaseStatsAttributeSet() const
{
	return BaseStatsAttributeSet;
}

UDensAbilityEnergyAttributeSet* ADensPlayerState::GetAbilityEnergyAttributeSet() const
{
	return AbilityEnergyAttributeSet;
}


//TEMP, replace later
UDensBaseWeaponStatsAttributeSet* ADensPlayerState::GetEquippedWeaponAttributeSet() const
{
	return EquippedWeaponAttributeSet;
}



FWeaponBasicAttributes& ADensPlayerState::GetSavedWeaponAttribute(EWeaponSlot Slot)
{

	return WeaponSavedAttributes[static_cast<int>(Slot)];
	
}




void ADensPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void ADensPlayerState::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		ActiveMagAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(EquippedWeaponAttributeSet->GetMagAmmoAttribute()).AddUObject(this, &ADensPlayerState::MagAmmoChanged);

		ActiveReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(EquippedWeaponAttributeSet->GetAmmoReservesAttribute()).AddUObject(this, &ADensPlayerState::ReserveAmmoChanged);
		
		GrenadeEnergyChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AbilityEnergyAttributeSet->GetGrenadeEnergyAttribute()).AddUObject(this, &ADensPlayerState::GrenadeEnergyChanged);

		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetHealthAttribute()).AddUObject(this, &ADensPlayerState::HealthChanged);

		ShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetShieldAttribute()).AddUObject(this, &ADensPlayerState::ShieldChanged);

		HandleFullHealthBarDelegateHandle = CombatAttributeSet->OnFullHealthBar.AddUObject(this, &ThisClass::HandleFullHealthBar);

		
		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Movement.Sprint")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADensPlayerState::SprintTagChanged);
		AbilitySystemComponent->RegisterGameplayTagEvent(DensGameplayTags::State_Movement_WeaponADS, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADensPlayerState::ADSTagChanged);


		
	}
}

void ADensPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void ADensPlayerState::Reset()
{
	Super::Reset();

	EquipmentManagerComponent->Reset();
	
	TArray<UPlayerStateComponent*> ModularComponents;
	GetComponents(ModularComponents);
	for (UPlayerStateComponent* Component : ModularComponents)
	{
		Component->Reset();
	}
}



void ADensPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME_CONDITION( ADensPlayerState, WeaponSavedAttributes,	COND_None );
}


void ADensPlayerState::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADensPlayerState::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADensPlayerState::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADensPlayerState::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}





#pragma region  AttributeGetters

float ADensPlayerState::GetEquippedWeaponMagAmmo() const
{
	return EquippedWeaponAttributeSet->GetMagAmmo();
}



float ADensPlayerState::GetGrenadeEnergy() const
{
	return AbilityEnergyAttributeSet->GetGrenadeEnergy();
}

float ADensPlayerState::GetHealth() const
{
	return CombatAttributeSet->GetHealth();
}

float ADensPlayerState::GetShield() const
{
	return CombatAttributeSet->GetShield();
}

float ADensPlayerState::GetHealthPercentage() const
{
	return CombatAttributeSet->GetHealth()/CombatAttributeSet->GetMaxHealth();
}


float ADensPlayerState::GetShieldPercentage() const
{
	return CombatAttributeSet->GetShield()/CombatAttributeSet->GetMaxShield();
}


#pragma endregion 




void ADensPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	TInlineComponentArray<UPlayerStateComponent*> PlayerStateComponents;
	GetComponents(PlayerStateComponents);
	for (UPlayerStateComponent* SourcePSComp : PlayerStateComponents)
	{
		if (UPlayerStateComponent* TargetComp = Cast<UPlayerStateComponent>(static_cast<UObject*>(FindObjectWithOuter(PlayerState, SourcePSComp->GetClass(), SourcePSComp->GetFName()))))
		{
			SourcePSComp->CopyProperties(TargetComp);
		}
	}
}


void ADensPlayerState::OnRep_SavedWeaponChanged()
{

	UnEquippedAmmoChanged();
	
	/*ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetUnequippedAmmo(WeaponSavedAttributes);
		}
	}*/
	
}

void ADensPlayerState::HandleFullHealthBar()
{
	//Call fade out HUD health bar client RPC
	FadeOutHUDHealthBar();
	
}


void ADensPlayerState::UnEquippedAmmoChanged()
{
	
	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetUnequippedAmmo(WeaponSavedAttributes);
		}
	}
}




void ADensPlayerState::MagAmmoChanged(const FOnAttributeChangeData & Data)
{
	float MagAmmo = Data.NewValue;

	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetMagAmmo(MagAmmo);
		}
	}
}

void ADensPlayerState::ReserveAmmoChanged(const FOnAttributeChangeData & Data)
{
	float ReserveAmmo = Data.NewValue;

	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetReservesAmmo(ReserveAmmo);
		}
	}
}




void ADensPlayerState::GrenadeEnergyChanged(const FOnAttributeChangeData& Data)
{

	float GrenadeEnergy = Data.NewValue;

	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetGrenadeEnergy(GrenadeEnergy);
		}
	}
}

void ADensPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{

	float HealthValue = Data.NewValue;

	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetHealthPercentage(HealthValue / CombatAttributeSet->GetMaxHealth());
		}
	}
}

void ADensPlayerState::ShieldChanged(const FOnAttributeChangeData& Data)
{

	float ShieldValue = Data.NewValue;

	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->SetShieldPercentage(ShieldValue / CombatAttributeSet->GetMaxShield());
		}
	}
}



void ADensPlayerState::FadeOutHUDHealthBar_Implementation()
{
	// Update the HUD
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensHUDWidget* HUD = PC->GetMainHUD();
		if (HUD)
		{
			HUD->FadeOutHealthBar();
		}
	}
}


void ADensPlayerState::SprintTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// Update the reticle
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensCombatReticleWidget* ReticleHUD = PC->GetReticleHUD();
		if (ReticleHUD)
		{
			bool TagActive = (NewCount == 0) ? false : true;
			ReticleHUD->OnSprintTagChanged(TagActive);
		}
	}
}



void ADensPlayerState::ADSTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// Update the reticle
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetOwner());
	if (PC)
	{
		UDensCombatReticleWidget* ReticleHUD = PC->GetReticleHUD();
		if (ReticleHUD)
		{
			bool TagActive = (NewCount == 0) ? false : true;
			ReticleHUD->OnADSTagChanged(TagActive);
		}
	}
}


