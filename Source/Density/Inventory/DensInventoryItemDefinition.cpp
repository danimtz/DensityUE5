// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensInventoryItemDefinition.h"


UDensInventoryItem::UDensInventoryItem()
{
	ItemInfo = nullptr;
	AbilitySetPlaceholder = 0;
	ItemStatsPlaceholder = 0;
	bIsEquipped = false;
}


void UDensInventoryItem::InitItem(FDensInventoryItemEntry& ItemEntry)
{
	ItemInfo = ItemEntry.ItemInfo;
	AbilitySetPlaceholder = ItemEntry.AbilitySetPlaceholder;
	ItemStatsPlaceholder = ItemEntry.ItemStatsPlaceholder;
	bIsEquipped = false;
}

EInventorySlot UDensInventoryItem::GetInventorySlot() const
{
	if(ItemInfo)
	{
		return ItemInfo->InventorySlot;
	}
	
	return EInventorySlot::NONE;
}



void UDensInventoryItem::OnItemEquipped()
{
	bIsEquipped = true;
	//Handle AbilitySets here?
}



void UDensInventoryItem::OnItemUnequipped()
{
	bIsEquipped = false;

	//Handle AbilitySets here?
}




UDensArmorInfo* UDensInventoryItemDefinition::GetArmorInfo()
{
	if(UDensArmorItemDefinition* ArmorDef = Cast<UDensArmorItemDefinition>(this))
	{
		return ArmorDef->ArmorInfo;
	}

	return nullptr;
}

TSubclassOf<UDensWeaponInstance> UDensInventoryItemDefinition::GetWeaponInstance()
{
	if(UDensWeaponItemDefinition* WeaponDef = Cast<UDensWeaponItemDefinition>(this))
	{
		return WeaponDef->WeaponInstance;
	}

	return nullptr;
}


