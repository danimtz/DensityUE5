// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Templates/SubclassOf.h"

#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayPrediction.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbilityTypes.h"


#include "DensGameplayAbilityActorInfo.generated.h"


class APlayerController;
class UAbilitySystemComponent;
class UAnimInstance;
class UAnimMontage;
class UDataTable;
class UGameplayAbility;
class UGameplayTask;
class UMovementComponent;
class USkeletalMeshComponent;
class UAbilitySystemComponent;





USTRUCT(BlueprintType)
struct DENSITY_API FDensGameplayAbilityActorInfo : public FGameplayAbilityActorInfo
{
	GENERATED_USTRUCT_BODY()

    virtual ~FDensGameplayAbilityActorInfo() {}


	virtual void InitFromActor(AActor* OwnerActor, AActor* AvatarActor, UAbilitySystemComponent* InAbilitySystemComponent);





	
};
