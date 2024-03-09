// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "DensStatusEffectsComponent.generated.h"


class UDensAbilitySystemComponent;

UCLASS()
class DENSITY_API UDensStatusEffectsComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensStatusEffectsComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	UFUNCTION()
	void OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC);

	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* WeakenEffectMaterial;
	
protected:
	
	void WeakenedTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	void SetActiveOverlayEffect(UMaterialInterface* EffectMaterial) const;
	
protected:

	TSet<UMaterialInterface*> ActiveEffects;

	//TODO implement a priority system for some of the effects
};
