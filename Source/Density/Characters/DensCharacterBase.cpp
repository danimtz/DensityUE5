// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensCharacterBase.h"

#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "DensGameplayTags.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/DensAbilityTypes.h"
#include "Ability/DensAbilitySet.h"
#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"
#include "Components/GameFrameworkComponentManager.h"

#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Damage.h"
#include "PlayerCharacters/DensCharacterMovementComponent.h"
#include "PlayerCharacters/DensStatusEffectsComponent.h"
#include "PlayerCharacters/DensCombatComponent.h"
#include "Tests/AutomationCommon.h"
#include "UserInterface/DensStatusBarComponent.h"

//Includes for FCharacterDeathInfo
#include "Ability/DensAbilityLibrary.h"
#include "Enemies/DensEnemy.h"
#include "Enemies/DensEnemyData.h"
#include "PlayerCharacters/DensCharacter.h"


FCharacterDeathInfo::FCharacterDeathInfo(AActor* DamageInstigator, const FGameplayEffectSpec& DamageEffectSpec)
{
	
	DeathAbilityName = FText::FromString("Misadventure"); 
	DeathCauserName = FText::FromString("Unknown"); 


	if(ADensEnemy* Enemy = Cast<ADensEnemy>(DamageInstigator))
	{
		DeathCauserName = Enemy->EnemyData->EnemyName;
	}

	if(ADensCharacter* EnemyGuardian = Cast<ADensCharacter>(DamageInstigator))
	{
		DeathCauserName = FText::FromString(EnemyGuardian->GetName());
	}

	
}




// Sets default values
ADensCharacterBase::ADensCharacterBase(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDensCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatusBarWidgetComponent = CreateDefaultSubobject<UDensStatusBarComponent>(FName("StatusBarComponent"));
	StatusBarWidgetComponent->SetupAttachment(RootComponent);
	StatusBarWidgetComponent->SetRelativeLocation(FVector(0, 0, 120));
	StatusBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	StatusBarWidgetComponent->SetDrawSize(FVector2D(500, 500));

	if(GetMesh())
	{
		CharacterMeshes.Add(GetMesh());
	}
	
	StatusEffectsComponent = CreateDefaultSubobject<UDensStatusEffectsComponent>(TEXT("Status Effects Component"));

	CombatComponent = CreateDefaultSubobject<UDensCombatComponent>(TEXT("Combat Component"));
	OnAbilitySystemInitializedDelegate.AddUObject(CombatComponent, &UDensCombatComponent::OnAbilitySystemComponentInitialized);
	
}

// Called when the game starts or when spawned
void ADensCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (UGameFrameworkComponentManager* ComponentManager = GetGameInstance()->GetSubsystem<UGameFrameworkComponentManager>())
	{
		ComponentManager->AddReceiver(this);
	}
}


UAbilitySystemComponent* ADensCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ADensCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION( ADensCharacterBase, MaxWalkSpeed,		COND_None );
	DOREPLIFETIME_CONDITION( ADensCharacterBase, MaxSprintSpeed,	COND_None );
}

UDensBaseStatsAttributeSet* ADensCharacterBase::GetBaseStatsAttributeSet() const
{
	return BaseStatsAttributeSet.Get();
}


UDensAbilitySystemComponent* ADensCharacterBase::GetDensAbilitySystemComponent() const
{
	return Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponent());
}





void ADensCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		DensASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADensCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADensCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADensCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}







UDensStatusBarComponent* ADensCharacterBase::GetStatusBar() const
{
	check(StatusBarWidgetComponent)
	
	return StatusBarWidgetComponent;
	
}

UDensCombatComponent* ADensCharacterBase::GetCombatComponent() const
{
	return CombatComponent;
}

/*
float ADensCharacterBase::GetMaxHealth() const
{
	return BaseStatsAttributeSet->GetMaxHealth();
	
}

float ADensCharacterBase::GetHealth() const
{
	return BaseStatsAttributeSet->GetHealth();
	
}*/



void ADensCharacterBase::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{

	UDensAbilitySystemComponent* InstigatorASC = UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(DamageInstigator);
	if (InstigatorASC)
	{
		// Send the Event.OnKill gameplay event through the instigator ability system. 
		{
			FGameplayEventData Payload;
			Payload.EventTag = DensGameplayTags::Event_OnKill;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			//FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			InstigatorASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		//Add gameplay cue for UI hit marker death
		{
			InstigatorASC->ExecuteGameplayCueLocalOnOwningClient(DensGameplayTags::GameplayCue_UI_HitMarkDeath, DamageEffectSpec.GetEffectContext());
		}
	}
	
	if (AbilitySystemComponent.Get())
	{
		// Send the Event.Death gameplay event through the owner's ability system. 
		{
			FGameplayEventData Payload;
			Payload.EventTag = DensGameplayTags::Event_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			//FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}



		{
			FGameplayCueParameters CueParameters;
			CueParameters.OriginalTag = DensGameplayTags::GameplayCue_Character_Death;
			FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());

			FGameplayTagContainer TagCont;
			FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(EffectContext);
			
			CueParameters.EffectContext = ContextHandle;
			CueParameters.Instigator = DamageInstigator;
			CueParameters.AggregatedSourceTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			CueParameters.AggregatedTargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			AbilitySystemComponent->ExecuteGameplayCue(DensGameplayTags::GameplayCue_Character_Death, DamageEffectSpec.GetEffectContext());
		}
	}


	FCharacterDeathInfo DeathInfo = FCharacterDeathInfo(DamageInstigator, DamageEffectSpec);


	DeathStarted(DeathInfo);
}

void ADensCharacterBase::HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (AbilitySystemComponent.Get())
	{
		// Send the Event.OnShieldBroken gameplay event through the owner's ability system. 
		{
			FGameplayEventData Payload;
			Payload.EventTag = DensGameplayTags::Event_OnShieldBroken;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;
			Payload.TargetData = UDensAbilityLibrary::GetTargetData(DamageEffectSpec.GetEffectContext());
			//FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		
	}

	//TODO add an OnEnemyShieldBroken event for the instigator
}

void ADensCharacterBase::HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser,
                                           const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (AbilitySystemComponent.Get())
	{
		// Send the Event.DamageTaken gameplay event through the owner's ability system. 
		{
			FGameplayEventData Payload;
			Payload.EventTag = DensGameplayTags::Event_OnDamageTaken;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;
			Payload.TargetData = UDensAbilityLibrary::GetTargetData(DamageEffectSpec.GetEffectContext());
			
			//FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		
		//Execute the gameplay cue for Damage Taken
		{
			FGameplayCueParameters CueParameters;
			CueParameters.OriginalTag = DensGameplayTags::GameplayCue_Character_DamageReact;
			

			FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());

			FGameplayTagContainer TagCont;
			//TagCont.AddTag(DensGameplayTags::Gameplay_Zone_Weakspot);
			//EffectContext->SetDamageTypeTags(TagCont);
			//EffectContext->SetIsCriticalHit(true);
			FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(EffectContext);
			
			CueParameters.EffectContext = ContextHandle;

			//FGameplayEffectContextHandle ContextHandl2e = DamageEffectSpec.GetEffectContext();
			
			AbilitySystemComponent->ExecuteGameplayCue(DensGameplayTags::GameplayCue_Character_DamageReact, DamageEffectSpec.GetEffectContext());
		}


		
		//Report Damage Event to AI system
		//If not player controlled call AI
		/*if(!IsPlayerControlled())
		{
			UAISense_Damage::ReportDamageEvent(this, this, DamageCauser, DamageMagnitude, );
		}*/
		
		
	}
	
}


void ADensCharacterBase::HandleStaggerThresholdCrossed(AActor* DamageInstigator, AActor* DamageCauser,
                                           const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	if (AbilitySystemComponent.Get())
	{
		// Send the Event.DamageTaken gameplay event through the owner's ability system. 
		{
			FGameplayEventData Payload;
			Payload.EventTag = DensGameplayTags::Event_OnStaggerThresholdCrossed;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;
			Payload.TargetData = UDensAbilityLibrary::GetTargetData(DamageEffectSpec.GetEffectContext());
			
			//FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		
	

		
	}
	
}




void ADensCharacterBase::Die()
{
	DeathStarted(FCharacterDeathInfo());
}

void ADensCharacterBase::Reset()
{
	Super::Reset();
}

void ADensCharacterBase::DeathStarted(FCharacterDeathInfo DeathInfo)
{
	

	if(OnCharacterDied.IsBound())
	{
		OnCharacterDied.Broadcast(this, DeathInfo);
	}
	
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCharacterMovement()->GravityScale = 0;
	//GetCharacterMovement()->Velocity = FVector(0);
	
	if (AbilitySystemComponent.IsValid())
	{
		//AbilitySystemComponent->CancelAllAbilities();
		//AbilitySystemComponent->Deac
		//TODO Add dead tag. Have dead tag cancel most abilities
		
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(DensGameplayTags::State_Effects);
		
		//TODO remove all effects
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		

		AbilitySystemComponent->AddLooseGameplayTag(DensGameplayTags::State_Dying);
		AbilitySystemComponent->AddReplicatedLooseGameplayTag(DensGameplayTags::State_Dying);
	}
	else
	{
		DeathFinished();
	}

	//if (DeathMontage)
	//{
		//PlayAnimMontage(DeathMontage);
	//}
	//else

	//Call on Death Event

	//OnDeath();
	
	{
		//
	}
	
}

void ADensCharacterBase::DeathFinished()
{

	UninitializeAbilitySystem();
	
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ADensCharacterBase::DestroyDueToDeath()
{
	Destroy();
}

void ADensCharacterBase::DyingTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{

	if(NewCount >0)
	{
		OnDyingTagAdded();
	}
	
}

void ADensCharacterBase::UninitializeAbilitySystem()
{
	//Unintialize ASC
	if (!AbilitySystemComponent.Get())
	{
		return;
	}

	
	
	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(DensGameplayTags::Gameplay);
	EffectTagsToRemove.AddTag(DensGameplayTags::State);
	AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
	
	AbilitySystemComponent->CancelAbilities(nullptr);
	AbilitySystemComponent->RemoveAllGameplayCues();
	
	
	AbilitySystemComponent->RemoveLooseGameplayTag(DensGameplayTags::State_Dying);
	AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(DensGameplayTags::State_Dying);
	AbilitySystemComponent->ClearAbilityInput();
	
	if (AbilitySystemComponent->GetOwnerActor() != nullptr)
	{
		AbilitySystemComponent->SetAvatarActor(nullptr);
	}
	else
	{
		// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
		AbilitySystemComponent->ClearActorInfo();
	}

	
	OnAbilitySystemUnInitializedDelegate.Broadcast(AbilitySystemComponent.Get());

	AbilitySystemComponent = nullptr;

}


float ADensCharacterBase::GetWalkSpeedMod() const
{
	if (BaseStatsAttributeSet.IsValid())
	{
		return BaseStatsAttributeSet->GetWalkSpeedMod();
	}

	return 0.0f;

}


float ADensCharacterBase::GetSprintSpeedMod() const
{
	if (BaseStatsAttributeSet.IsValid())
	{
		return BaseStatsAttributeSet->GetSprintSpeedMod();
		
	}

	return 0.0f;

}


float ADensCharacterBase::GetWalkSpeed() const
{
	return MaxWalkSpeed;
}


float ADensCharacterBase::GetSprintSpeed() const
{
	return MaxSprintSpeed;
}



bool ADensCharacterBase::
IsSprinting() const
{
	if (DensCharacterMovementComponent)
	{
		return DensCharacterMovementComponent->bIsSprinting;
	}
	return false;
}
