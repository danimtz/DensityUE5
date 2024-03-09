// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "DensInputConfig.generated.h"



class UInputAction;
class UObject;
/**
 * FDensInputActionTagPair
 *
 *	Maps input action asset to InputTag gameplay tag
 */
USTRUCT(BlueprintType)
struct DENSITY_API FDensInputActionTagPair
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
		FGameplayTag InputTag;

};


/**
* UDensInputConfig
*
*	Non-mutable data asset containing input config properties
*/
UCLASS(BlueprintType, Const)
class DENSITY_API UDensInputConfig : public UDataAsset
{
	GENERATED_BODY()


public:

	UDensInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Density|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Density|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:

	//Owner input actions. Manually set
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDensInputActionTagPair> NativeInputActionTagPairs;

	//Owner input actions. Mapped to gameplay tag and bound to abilities with matching input tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FDensInputActionTagPair> AbilityInputActionTagPairs;

};
