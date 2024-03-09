// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensDamagableActor.h"

#include "AbilitySystemGlobals.h"
#include "DensGameplayTags.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/AttributeSets/DensCombatAttributeSet.h"
#include "..\Characters\PlayerCharacters\DensCombatComponent.h"
#include "Ability/DensAbilityTypes.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADensDamagableActor::ADensDamagableActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UDensAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	CombatAttributeSet = CreateDefaultSubobject<UDensCombatAttributeSet>(TEXT("CombatAttributeSet"));
	CombatComponent = CreateDefaultSubobject<UDensCombatComponent>(TEXT("CombatComponent"));
	
	OnAbilitySystemInitializedDelegate.AddUObject(CombatComponent, &UDensCombatComponent::OnAbilitySystemComponentInitialized);

	OnAbilitySystemInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemInitialized);
}


//TODO implement lazy loading of ASC?


UDensAbilitySystemComponent* ADensDamagableActor::GetDensAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


UAbilitySystemComponent* ADensDamagableActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADensDamagableActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		DensASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADensDamagableActor::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADensDamagableActor::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADensDamagableActor::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UDensAbilitySystemComponent* DensASC = GetDensAbilitySystemComponent())
	{
		return DensASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void ADensDamagableActor::BeginPlay()
{
	Super::BeginPlay();


	if (AbilitySystemComponent.Get())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		

		OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent);
	}

	
}

void ADensDamagableActor::InitializeAttributes()
{
	CombatAttributeSet->SetMaxHealth(MaxHealth);
	CombatAttributeSet->SetHealth(MaxHealth);
	CombatAttributeSet->SetMaxShield(MaxShield);
	CombatAttributeSet->SetShield(MaxShield);
}

void ADensDamagableActor::InitializeAbilities()
{

	if(AbilitySet)
	{
		AbilitySet->AddToASC(AbilitySystemComponent, &GrantedAbilityHandles);
	}
	
}

void ADensDamagableActor::OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC)
{
	InitializeAttributes();
	InitializeAbilities();

	CombatComponent->OnDamageTaken.AddUniqueDynamic(this, &ThisClass::HandleDamageTaken);
}


void ADensDamagableActor::HandleDamageTaken(AActor* DamageInstigator, AActor* DamageCauser,
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

			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		
		//Execute the gameplay cue for Damage Taken
		{
			FGameplayCueParameters CueParameters;
			CueParameters.OriginalTag = DensGameplayTags::GameplayCue_Actor_DamageReact;
			FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
			FGameplayTagContainer TagCont;
			FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(EffectContext);
			CueParameters.EffectContext = ContextHandle;
			
			AbilitySystemComponent->ExecuteGameplayCue(DensGameplayTags::GameplayCue_Actor_DamageReact, DamageEffectSpec.GetEffectContext());
		}
		
	}
}
