// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensStatusEffectsComponent.h"

#include "DensCharacter.h"
#include "Ability/DensAbilitySystemComponent.h"


// Sets default values for this component's properties
UDensStatusEffectsComponent::UDensStatusEffectsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;
	
}

void UDensStatusEffectsComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	ADensCharacterBase* OwnerDensCharacter = GetOwner<ADensCharacterBase>();
	OwnerDensCharacter->OnAbilitySystemInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemInitialized);

	
}

void UDensStatusEffectsComponent::OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC)
{
	ADensCharacterBase* OwnerDensCharacter = GetOwner<ADensCharacterBase>();
	UDensAbilitySystemComponent* DensASC = OwnerDensCharacter->GetDensAbilitySystemComponent();
	
	DensASC->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Effects.Weakened")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::WeakenedTagChanged);
	
	
}


void UDensStatusEffectsComponent::WeakenedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	
	if(NewCount <= 0)
	{
		ActiveEffects.Remove(WeakenEffectMaterial);
		SetActiveOverlayEffect(nullptr);
	}
	else
	{
		ActiveEffects.Add(WeakenEffectMaterial);
		SetActiveOverlayEffect(WeakenEffectMaterial);
	}
	
	
}

void UDensStatusEffectsComponent::SetActiveOverlayEffect(UMaterialInterface* EffectMaterial) const
{

	if(EffectMaterial == nullptr)
	{
		if(!ActiveEffects.IsEmpty())
		{
			//TODO check other effects by priority and set Effect material to the highest priority active effect
		}
	}

	
	ADensCharacterBase* OwnerDensCharacter = GetOwner<ADensCharacterBase>();
	TArray<USkeletalMeshComponent*> Meshes = OwnerDensCharacter->GetCharacterMeshes();

	
	for(USkeletalMeshComponent* Mesh : Meshes)
	{
		Mesh->SetOverlayMaterial(EffectMaterial);
	}
	
}
