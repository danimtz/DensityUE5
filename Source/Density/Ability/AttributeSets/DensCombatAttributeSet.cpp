// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#include "DensCombatAttributeSet.h"

#include "DensGameplayTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/DensAbilityTypes.h"
#include "Characters/DensCharacterBase.h"

#include "Net/UnrealNetwork.h"
#include "Player/DensPlayerController.h"
#include "UserInterface/Reticles/DensCombatReticleWidget.h"


//Initialize attributes.
UDensCombatAttributeSet::UDensCombatAttributeSet() :
Health(70.0f), MaxHealth(70.0f), Shield(115.0f), MaxShield(115.0f), CritDamageModifier(1.0f), StaggerThreshold(1000.0f), CurrentStagger(0.0f), OutgoingStaggerDamage(0.0f), IncomingStaggerDamage(0.0f),
OutgoingDamage(0.0f), IncomingDamage(0.0f), OutgoingEmpowerBuff(1.0f), OutgoingSurgeBuff(1.0f), IncomingWeakenDebuff(1.0f), IncomingDamageResistBuff(1.0f)
{
	bOutOfHealth = false;
	bOutOfShield = false;
	bIsFullHealthBar = false;
	bStaggerThresholdCrossed = false;
}

void UDensCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttributeOnChanged(Attribute, NewValue);
}

void UDensCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttributeOnChanged(Attribute, NewValue);
}

void UDensCombatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMaxShieldAttribute())
	{
		//TODO Change Health to ensure it is not larger than new Max health

		float ChangePercent = NewValue/OldValue;

		float NewShield = GetShield() * ChangePercent;

		SetShield(NewShield);
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}

	if (bOutOfShield && (GetShield() > 0.0f))
	{
		bOutOfShield = false;
	}

	if (bIsFullHealthBar && ( (GetShield() < GetMaxShield()) || (GetHealth() < GetMaxHealth()) )  )
	{
		bIsFullHealthBar = false;
	}

	if(bStaggerThresholdCrossed && GetCurrentStagger() < GetStaggerThreshold())
	{
		bStaggerThresholdCrossed = false;
	}
}

void UDensCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);


	// Get the Target actor (should be the owner)
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	UDensAbilitySystemComponent* TargetASC = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		TargetASC = UDensAbilitySystemComponent::GetAbilitySystemComponentFromActor(TargetActor);
	}


	//Get Source Target actor //From GASDocumentation
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = Context.GetOriginalInstigatorAbilitySystemComponent();
	UDensAbilitySystemComponent* SourceDensASC = Cast<UDensAbilitySystemComponent>(SourceASC);

	
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	ADensCharacterBase* SourceCharacter = nullptr;
	if (SourceASC && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<ADensCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<ADensCharacterBase>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	
	
	float LocalDamage = 0.0f;
	if(Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		LocalDamage = GetDamageTaken();
		SetDamageTaken(0.0f);
		if(LocalDamage > 0.0f)
		{

			// Apply the damage to the shield first. change and then clamp it
			const float NewShield = GetShield() - LocalDamage;
			SetShield(FMath::Clamp(NewShield, 0.0f, GetMaxShield()));


			if(NewShield < 0.0f)
			{
				// Leftover damage that got through shield will apply to health
				const float NewHealth = GetHealth() + NewShield;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			}
			
		}


		// Show damage number for the Source player unless it was self damage //TODO MAYBE THIS SHOULD BE A GAMEPLAY CUE?
		if (SourceActor != TargetActor)
		{
			ADensPlayerController* PC = Cast<ADensPlayerController>(SourceController);
			if (PC && TargetASC)
			{
				check(Context.GetHitResult());

				//Check if empowered damage from context
				bool bIsEmpowered = false;


				const FDensGameplayEffectContext* DensContext = static_cast<const FDensGameplayEffectContext*>(Context.Get());
				if (DensContext)
				{
					 bool WeakenedTag = TargetASC->HasMatchingGameplayTag(DensGameplayTags::State_Effects_Weakened);
					 bIsEmpowered =  DensContext->IsCriticalHit() ||  WeakenedTag;
					 
					
				}

				//TODO each damage number is a reliable RPC currently. Probably should batch these
				//TODO damage numbers show up on first hit location for grenades
				//Maybe use gameplay cues for this?
				//TODO maybe do this from component and not attribute set?
				PC->ShowDamageNumber(LocalDamage, TargetActor, *Context.GetHitResult(), bIsEmpowered);
				
				
				
			}
		}


		// Send the Event.OnDamageTaken gameplay event through the targets's ability system.
		if(OnDamageTaken.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnDamageTaken.Broadcast(Instigator, Causer, Data.EffectSpec, LocalDamage);
		}
	}


	//Handle Stagger
	float LocalStaggerDamage = 0.0f;
	if(Data.EvaluatedData.Attribute == GetStaggerDamageTakenAttribute())
	{
		LocalStaggerDamage = GetStaggerDamageTaken();

		const float NewStagger = GetCurrentStagger() + LocalStaggerDamage;
		SetCurrentStagger(NewStagger);
		
	}

	

	//Handle running out of shield
	if(GetShield() <= 0.0f && !bOutOfShield)
	{
		if(OnOutOfShield.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfShield.Broadcast(Instigator, Causer, Data.EffectSpec, LocalDamage);
		}
	}
	
	//Handle running out of health
	if(GetHealth() <= 0.0f && !bOutOfHealth)
	{
		if(OnOutOfHealth.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, LocalDamage);
		}
	}


	//Handle stagger threshold hit
	if(GetCurrentStagger() >= GetStaggerThreshold() && !bStaggerThresholdCrossed)
	{
		if(OnStaggerThresholdCrossed.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnStaggerThresholdCrossed.Broadcast(Instigator, Causer, Data.EffectSpec, LocalStaggerDamage);
		}
	}
		

	//TODO OnShieldNotEmpty

	
	//Handle health and shield full
	if((GetShield() >= GetMaxShield()) && (GetHealth() >= GetMaxHealth()) && !bIsFullHealthBar)
	{
		if(OnFullHealthBar.IsBound())
		{
			OnFullHealthBar.Broadcast();
		}
	}


	
	//Check if something changed health
	bOutOfHealth = (GetHealth() <= 0.0f);

	bOutOfShield = (GetShield() <= 0.0f);

	bIsFullHealthBar = (GetShield() >= GetMaxShield()) && (GetHealth() >= GetMaxHealth());

	bStaggerThresholdCrossed = (GetCurrentStagger() >= GetStaggerThreshold());
}

void UDensCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, CritDamageModifier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, CurrentStagger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDensCombatAttributeSet, StaggerThreshold, COND_None, REPNOTIFY_Always);
}

void UDensCombatAttributeSet::ClampAttributeOnChanged(const FGameplayAttribute& Attribute, float& NewValue) const
{

	if (Attribute == GetHealthAttribute())
	{
		//Health should not be negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		//MaxHealth should not drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}

	if (Attribute == GetShieldAttribute())
	{
		//Shield should not be negative or above max shield.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}
	
}



void UDensCombatAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, Health, OldHealth);
}

void UDensCombatAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, MaxHealth, OldMaxHealth);
}

void UDensCombatAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, Shield, OldShield);
}

void UDensCombatAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, MaxShield, OldMaxShield);
}

void UDensCombatAttributeSet::OnRep_CritDamageModifier(const FGameplayAttributeData& OldCritDamageModifier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, CritDamageModifier, OldCritDamageModifier);
}


void UDensCombatAttributeSet::OnRep_StaggerThreshold(const FGameplayAttributeData& OldStaggerThreshold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, StaggerThreshold, OldStaggerThreshold);
}

void UDensCombatAttributeSet::OnRep_CurrentStagger(const FGameplayAttributeData& OldCurrentStagger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDensCombatAttributeSet, CurrentStagger, OldCurrentStagger);
}

