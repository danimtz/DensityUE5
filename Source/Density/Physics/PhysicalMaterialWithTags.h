// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicalMaterialWithTags.generated.h"

/**
 * 
 */
UCLASS()
class UPhysicalMaterialWithTags : public UPhysicalMaterial
{
	GENERATED_BODY()
	
public:
	UPhysicalMaterialWithTags(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// A container of gameplay tags that game code can use to reason about this physical material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicalProperties)
	FGameplayTagContainer Tags;
};
