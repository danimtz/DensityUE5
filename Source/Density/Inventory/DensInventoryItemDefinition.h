// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DensInventoryItemDefinition.generated.h"


class UDensWeaponInstance;
class UDensArmorInfo;
class UDensInventoryItemDefinition;


UENUM(BlueprintType, meta = (Bitflags))
enum class EInventorySlot : uint8
{
	NONE	   = 0 UMETA(Hidden),
	KineticWeapon = 1 << 0	UMETA(DisplayName = "Kinetic"),
	EnergyWeapon = 1 << 1	UMETA(DisplayName = "Energy"),
	HeavyWeapon	= 1 << 2	UMETA(DisplayName = "Heavy"),
	
	HeadArmor	= 1 << 3	UMETA(DisplayName = "Head"),
	ArmsArmor	= 1 << 4	UMETA(DisplayName = "Arms"),
	ChestArmor	= 1 << 5	UMETA(DisplayName = "Chest"),
	LegArmor	= 1 << 6	UMETA(DisplayName = "Leg"),
	ClassItemArmor = 1 << 7  UMETA(DisplayName = "ClassItem")
};
ENUM_CLASS_FLAGS(EInventorySlot);








/*
 * Inventory UOBJECT used by the EquipmentManagerComponent and InventoryManagerComponents
 */
UCLASS(BlueprintType)
class DENSITY_API UDensInventoryItem : public UObject
{
	GENERATED_BODY()


public:
	UDensInventoryItem();

	void InitItem(FDensInventoryItemEntry& ItemEntry);
	
	//TODO Add function to Funciton library to call this from blueprints
	EInventorySlot GetInventorySlot() const;
	
	void OnItemEquipped();

	void OnItemUnequipped();

	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDensInventoryItemDefinition> ItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int AbilitySetPlaceholder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ItemStatsPlaceholder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsEquipped;
	


};


/*
 * Data table row entry, Used kind of as a placeholder for now. Inventory data table is used as a makeshift database that the player would have online
 */
USTRUCT(BlueprintType)
struct DENSITY_API FDensInventoryItemEntry : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDensInventoryItemDefinition> ItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int AbilitySetPlaceholder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ItemStatsPlaceholder;
};




/**
 * Inventory Item Data Asset.
 */
UCLASS(BlueprintType)
class DENSITY_API UDensInventoryItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:


	//Item Icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> ItemIcon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EInventorySlot InventorySlot;


	//Get data from subclasses else return nullptr
	UFUNCTION()
	UDensArmorInfo* GetArmorInfo();
	
	UFUNCTION()
	TSubclassOf<UDensWeaponInstance> GetWeaponInstance();
	
};


UCLASS(BlueprintType)
class DENSITY_API UDensArmorItemDefinition : public UDensInventoryItemDefinition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDensArmorInfo> ArmorInfo;
	
};


UCLASS(BlueprintType)
class DENSITY_API UDensWeaponItemDefinition : public UDensInventoryItemDefinition
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UDensWeaponInstance> WeaponInstance;
	
};




