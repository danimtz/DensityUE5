// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensPlayerController.h"

#include "DensPlayerState.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Characters/DensCharacterBase.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Inventory/DensEquipmentManagerComponent.h"
#include "Inventory/DensInventoryManagerComponent.h"
#include "UserInterface/DeathScreenWidget.h"
#include "UserInterface/DensFloatingCombatTextComponent.h"
#include "UserInterface/DensHUDWidget.h"
#include "UserInterface/Reticles/DensCombatReticleWidget.h"



ADensPlayerController::ADensPlayerController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

	CombatTextComponent = CreateDefaultSubobject<UDensFloatingCombatTextComponent>(FName("CombatTextManager"));

	InventoryManagerComponent = CreateDefaultSubobject<UDensInventoryManagerComponent>(FName("InventoryManager"));
	//AddOwnedComponent(FloatingCombatTextComponent);
	
}



ADensPlayerState* ADensPlayerController::GetDensPlayerState() const
{
	return CastChecked<ADensPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}



UDensAbilitySystemComponent* ADensPlayerController::GetDensAbilitySystemComponent() const
{
	const ADensPlayerState* DensPS = GetDensPlayerState();
	return (DensPS ? DensPS->GetDensAbilitySystemComponent() : nullptr);
}


//Call this from Character on OnPossess or OnRep_PlayerState?
void ADensPlayerController::InitHUD()
{
	//Init only once
	if(HUDWidget)
	{
		return;
	}

	if (!HUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing HUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
	if (!PS)
	{
		return;
	}

	//Remove player dead UI
	if(DeathScreenWidget)
	{
		DeathScreenWidget->RemoveFromParent();
		DeathScreenWidget = nullptr;
	}
	
	
	//TODO Init hud widget
	
	HUDWidget = CreateWidget<UDensHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();

	//Init attributes //TODO Add rest of attributes
	HUDWidget->SetGrenadeEnergy(PS->GetGrenadeEnergy());
	HUDWidget->SetMagAmmo(PS->GetEquippedWeaponMagAmmo());

	HUDWidget->SetHealthPercentage(PS->GetHealthPercentage());
	HUDWidget->SetShieldPercentage(PS->GetShieldPercentage());


	
	ReticleWidget = CreateWidget<UDensCombatReticleWidget>(this, ReticleWidgetClass);
	ReticleWidget->AddToViewport(10);
	


	
	OnHUDInitialized();
	
}



UDensHUDWidget* ADensPlayerController::GetMainHUD()
{
	return HUDWidget;
}

UDensCombatReticleWidget* ADensPlayerController::GetReticleHUD()
{
	return ReticleWidget;
}

void ADensPlayerController::ShowDamageNumber(float DamageAmount, AActor* TargetCharacter, const FHitResult& HitResult, bool bDisplayEmpowered)
{
	/*if (TargetCharacter && DamageNumberWidgetClass)
	{
		UDensDamageNumberWidgetComponent* DamageText = NewObject<UDensDamageNumberWidgetComponent>(TargetCharacter, DamageNumberWidgetClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->SetDamageText(DamageAmount, HitResult, IsCriticalHit);
	}*/

	if (TargetCharacter && CombatTextComponent)
	{
		
		CombatTextComponent->AddFloatingText_CLIENT(DamageAmount, bDisplayEmpowered, HitResult.ImpactPoint);
		
	}
	
}

UDensInventoryManagerComponent* ADensPlayerController::GetInventoryManagerComponent()
{
	return InventoryManagerComponent;
}

void ADensPlayerController::ToggleIncomingWipeWidget_Implementation(bool bWidgetActive)
{
	
	if(bWidgetActive)
	{
		BeginIncomingWipeHUDWidget();
	}
	else
	{
		RemoveIncomingWipeHUDWidget();
	}
	
	
}

void ADensPlayerController::OnPlayerDeath()
{
	//Broadcast delegate to most likely the gamemode
	if(PlayerDiedDelegate.IsBound())
	{
		PlayerDiedDelegate.Broadcast();
	}
}


void ADensPlayerController::OnPlayerDeath_CLIENT_Implementation(FCharacterDeathInfo DeathInfo)
{
	//Remove HUD and replace with death hud
	if(IsLocalController())
	{
		if(GetMainHUD())
		{
			GetMainHUD()->RemoveFromParent();
		}
		
		if(GetReticleHUD()){
			GetReticleHUD()->RemoveFromParent();
		}
		HUDWidget = nullptr;
		ReticleWidget = nullptr;


		if (!DeathScreenWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("%s() Missing DeathScreenWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
			return;
		}

		DeathScreenWidget = CreateWidget<UDeathScreenWidget>(this, DeathScreenWidgetClass);
		DeathScreenWidget->AddToViewport();

		DeathScreenWidget->DisplayDeathInfo(DeathInfo);
		
	}
	
}


//Server only
void ADensPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	ADensCharacter* DensCharacter = Cast<ADensCharacter>(InPawn);
	if(DensCharacter)
	{
		ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
		
		
		//If Standalone or Server
		if (IsLocalController())
		{
			InitHUD();
			GetMainHUD()->FadeOutHealthBar(); //TODO add this to future OnPlayerSpawn delegate or something the sort
		}
		
		PS->GetEquipmentManagerComponent()->UpdatePlayerEquipment();
		PS->GetEquipmentManagerComponent()->RefreshPlayerEquipment();
		
		GetInventoryManagerComponent()->InitInventory();
		
		if(PlayerRespawnDelegate.IsBound())
		{
			PlayerRespawnDelegate.Broadcast(DensCharacter);
		}
		
	}

	
}





void ADensPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}


void ADensPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		DensASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}


void ADensPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For rare cases where the PlayerState is repped before the Hero is possessed.
	InitHUD();
}