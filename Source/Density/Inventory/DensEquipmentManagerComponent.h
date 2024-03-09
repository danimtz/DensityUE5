// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DensInventoryItemDefinition.h"

#include "DensEquipmentManagerComponent.generated.h"


class UDensWeaponItemDefinition;
class UDensArmorItemDefinition;
class UDensInventoryItemDefinition;
class UDensAbilitySystemComponent;



/*
 * Manages player equipment. Handles equipping weapon/armor from inventory to active equipment
 *
 *
 */

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class DENSITY_API UDensEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensEquipmentManagerComponent();

	virtual void InitializeComponent() override;
	

	void EquipInventoryItem(UDensInventoryItem* InvItem);
	
	
	void EquipArmor(UDensInventoryItem* NewArmor);
	void EquipWeapon(UDensInventoryItem* NewWeapon);

	
	void UpdatePlayerEquipment();

	//Used On Respawn to reinit weapons/Armor without resetting the,
	void RefreshPlayerEquipment();

	void Reset();
	
protected:
	
	

	
	
protected:

	

	
	//Weapons
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> KineticWeapon;

	UPROPERTY()
	TObjectPtr<UDensInventoryItem> EnergyWeapon;
	
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> HeavyWeapon;

	

	//Armor
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> HeadArmor;
	
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> ChestArmor;
	
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> ArmsArmor;
	
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> LegArmor;
	
	UPROPERTY()
	TObjectPtr<UDensInventoryItem> ClassItemArmor;


	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "EInventorySlot"))
	uint8 ItemDirtyFlag = 0;
};
