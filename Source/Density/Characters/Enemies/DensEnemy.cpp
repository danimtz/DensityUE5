// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensEnemy.h"

#include "DensEnemyData.h"
#include "DensGameplayTags.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"
#include "..\PlayerCharacters\DensCombatComponent.h"
#include "Ability/DensAbilitySet.h"
#include "Characters/PlayerCharacters/DensCharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UserInterface/DensEnemyStatusBarWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DensPlayerController.h"
#include "UserInterface/DensHUDWidget.h"
#include "UserInterface/DensStatusBarComponent.h"

// Sets default values
ADensEnemy::ADensEnemy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	DensCharacterMovementComponent = Cast<UDensCharacterMovementComponent>(GetCharacterMovement());
	if (DensCharacterMovementComponent)
	{
		DensCharacterMovementComponent->GravityScale = 1.5;
		DensCharacterMovementComponent->MaxAcceleration = 3048;
		DensCharacterMovementComponent->BrakingFrictionFactor = 0;
		DensCharacterMovementComponent->bUseSeparateBrakingFriction = true;
		DensCharacterMovementComponent->PerchRadiusThreshold = 30.0f;
		DensCharacterMovementComponent->bUseFlatBaseForFloorChecks = true;
		DensCharacterMovementComponent->JumpZVelocity = 750;
		DensCharacterMovementComponent->BrakingDecelerationFalling = 200;
		DensCharacterMovementComponent->AirControl = 0.25;
		DensCharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
		//DensCharacterMovementComponent->Hold
		
	}
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponentHardRef = CreateDefaultSubobject<UDensAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponentHardRef->SetIsReplicated(true);

	AbilitySystemComponentHardRef->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	//Set parent WeakPtr to hard ref
	AbilitySystemComponent = AbilitySystemComponentHardRef;
	
	BaseStatsAttributeSetHardRef = CreateDefaultSubobject<UDensBaseStatsAttributeSet>(TEXT("BaseStatsAttributeSet"));
	BaseStatsAttributeSet = BaseStatsAttributeSetHardRef;

	CombatAttributeSet = CreateDefaultSubobject<UDensCombatAttributeSet>(TEXT("CombatAttributeSett"));
	

	
	//Initialize UI
	//UIEnemyStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	//UIEnemyStatusBarComponent->SetupAttachment(RootComponent);
	//UIEnemyStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	//UIEnemyStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//UIEnemyStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	//UIEnemyStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/UI/UI_FloatingStatusBar_Minion.UI_FloatingStatusBar_Minion_C"));
	//if (!UIEnemyStatusBarClass)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	//}
}





// Called when the game starts or when spawned
void ADensEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Init ASC
	if (AbilitySystemComponent.Get())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		InitializeAbilities();
		//AddStartupEffects();
		//AddCharacterAbilities();

		OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent.Get());
		
		//Attribute delegate callbacks here
		CombatComponent->OnOutOfHealth.AddUniqueDynamic(this, &ThisClass::HandleOutOfHealth);
		CombatComponent->OnDamageTaken.AddUniqueDynamic(this, &ThisClass::HandleDamageTaken);
		
		CombatComponent->OnStaggerThresholdCrossed.AddUniqueDynamic(this, &ThisClass::HandleStaggerThresholdCrossed);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(DensGameplayTags::State_Dying, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DyingTagChanged);

		Faction = EnemyData->Faction;
		EnemyFactions = EnemyData->EnemyFactions;
		
		// Setup UI status bar for local players here
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->IsLocalPlayerController())
		{

			//Special case boss enemies show their health bar on the players main HUD on the PC
			if(EnemyData->EnemyTier == EEnemyTier::Boss)
			{
				if(ADensPlayerController* DensPC = Cast<ADensPlayerController>(PC))
				{ 
					if(DensPC->GetMainHUD())
					{
						DensPC->GetMainHUD()->InitBossHealthBar(EnemyData);
					}
					

					
				}
			}
			else if(EnemyData->StatusBarClass)
			{
				StatusBarWidgetComponent->InitializeStatusBar(EnemyData->StatusBarClass, EnemyData->EnemyName, CombatComponent->GetHealth() / CombatComponent->GetMaxHealth());
			
			}

			AbilitySystemComponent->RegisterGameplayTagEvent(DensGameplayTags::State_Combat_Immune, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::ImmuneTagChanged);

			
			//This could be managed through the CombatComponent TODO
			HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetHealthAttribute()).AddUObject(this, &ADensEnemy::HealthChanged);
		}
	}


	if(EnemyData->SpawnAnimation)
	{
		PlayAnimMontage(EnemyData->SpawnAnimation);
	}

	
}

void ADensEnemy::InitializeAttributes()
{
	if(EnemyData)
	{
		CombatAttributeSet->InitHealth(EnemyData->Health);
		
		CombatAttributeSet->InitMaxHealth(EnemyData->Health);

		CombatAttributeSet->InitStaggerThreshold(EnemyData->StaggerThreshold);
		
		CombatAttributeSet->InitCritDamageModifier(EnemyData->CritDamageModifier);

		
		//Set shields to 0
		CombatAttributeSet->InitMaxShield(0);
		CombatAttributeSet->InitShield(0);
	}

	//TODO Error check Enemy Data
}



void ADensEnemy::InitializeAbilities()
{

	if (AbilitySystemComponent.IsValid())
	{
		
		UDensAbilitySystemComponent* DensASC = AbilitySystemComponent.Get();
		if (EnemyData->DefaultAbilitySet)
		{
			EnemyData->DefaultAbilitySet->AddToASC(DensASC, &GrantedAbilityHandles);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilitySet not found or not set for DensEnemy  [%s] ."), *GetNameSafe(this));
		}

		for(auto AbilitySet : AdditionalAbilitySets)
		{
			if(AbilitySet)
			{
				AbilitySet->AddToASC(DensASC, &GrantedAbilityHandles);
			}
		}
		
	}
}


void ADensEnemy::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	
	//Update enemy health
	if(EnemyData->EnemyTier != EEnemyTier::Boss)
	{
		if (StatusBarWidgetComponent->IsInitialized())
		{
			StatusBarWidgetComponent->SetHealthPercentage(Health / CombatComponent->GetMaxHealth());
		}

	}
	else
	{
		//TODO I dont like this, how it accesses the boss hps like this
		ADensPlayerController* DensPC = Cast<ADensPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (DensPC && DensPC->IsLocalPlayerController())
		{
			if(DensPC->GetMainHUD())
			{
				DensPC->GetMainHUD()->SetBossHealthPercentage(Health / CombatComponent->GetMaxHealth());
			}
			
		}
			
	}

}

void ADensEnemy::ImmuneTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{

	if(EnemyData->EnemyTier == EEnemyTier::Boss)
	{
		//TODO I dont like this, how it accesses the boss hp bar habing to cast to player controller like this
		ADensPlayerController* DensPC = Cast<ADensPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (DensPC && DensPC->IsLocalPlayerController())
		{
			bool IsImmune = (NewCount <=0) ? false : true;
			DensPC->GetMainHUD()->SetBossHealthImmune(IsImmune);
		}
	}

	
}



