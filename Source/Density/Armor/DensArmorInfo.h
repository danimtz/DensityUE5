// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DensArmorInfo.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	ARMORSLOT_Head	UMETA(DisplayName = "Head"),
	ARMORSLOT_Arms	UMETA(DisplayName = "Arms"),
	ARMORSLOT_Chest	UMETA(DisplayName = "Chest"),
	ARMORSLOT_Legs	UMETA(DisplayName = "Legs"),
	ARMORSLOT_ClassItem UMETA(DisplayName = "ClassItem"),
};


UCLASS(BlueprintType, Const)
class DENSITY_API UDensArmorInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//Armor slot
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	EArmorSlot ArmorSlot;

	
	
	//Armor mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	TObjectPtr<USkeletalMesh> ArmorMesh;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FirstPerson")
	TObjectPtr<USkeletalMesh> ArmorMesh_FirstPerson;

	
};
