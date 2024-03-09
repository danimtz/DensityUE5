// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensInventoryManagerComponent.h"

#include "DensEquipmentManagerComponent.h"
#include "DensEquipmentManagerComponent.h"
#include "DensInventoryItemDefinition.h"
#include "Player/DensPlayerController.h"
#include "Player/DensPlayerState.h"


// Sets default values for this component's properties
UDensInventoryManagerComponent::UDensInventoryManagerComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	bIsInventoryInnited = false;
}

void UDensInventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();


	InitInventoryFromDataTable();
	
}



void UDensInventoryManagerComponent::InitInventoryFromDataTable()
{

	if(InventoryTable)
	{

		//Put items in inventory array
		for(auto TableEntry : InventoryTable->GetRowMap() )
		{
			FDensInventoryItemEntry* Entry = reinterpret_cast<FDensInventoryItemEntry*>(TableEntry.Value);
			UDensInventoryItem* Item = NewObject<UDensInventoryItem>();
			Item->InitItem(*Entry);
			InventoryItems.Emplace(Item);
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryTable in InventoryManagerComponent in PlayerController is not valid [%s]"),  *GetNameSafe(InventoryTable));
	}


}

//Initialize inventory. Equip first item found for each slot
void UDensInventoryManagerComponent::InitInventory()
{

	if(bIsInventoryInnited)
	{
		return;
	}
	bIsInventoryInnited = true;
	
	//TODO Add this to player controller so that i can call DensPC->GetEquipmentManagerComponent();
	ADensPlayerController* DensPC = GetOwner<ADensPlayerController>();
	ADensPlayerState* DensPS = DensPC->GetPlayerState<ADensPlayerState>();
	UDensEquipmentManagerComponent* EquipmentManager = DensPS->GetEquipmentManagerComponent();

	check(EquipmentManager);
	
	uint8 ItemFoundFlag = 0;
	

	for(UDensInventoryItem* Item : InventoryItems)
	{

		EInventorySlot Slot = Item->GetInventorySlot();
		
		switch(Slot)
		{
			case EInventorySlot::KineticWeapon:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::KineticWeapon)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::KineticWeapon);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
			case EInventorySlot::EnergyWeapon:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::EnergyWeapon)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::EnergyWeapon);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
			case EInventorySlot::HeavyWeapon:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::HeavyWeapon)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::HeavyWeapon);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
				
			case EInventorySlot::HeadArmor:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::HeadArmor)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::HeadArmor);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
				
			case EInventorySlot::ArmsArmor:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::ArmsArmor)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::ArmsArmor);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
			case EInventorySlot::ChestArmor:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::ChestArmor)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::ChestArmor);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
			case EInventorySlot::LegArmor:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::LegArmor)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::LegArmor);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
			case EInventorySlot::ClassItemArmor:
				if (!(ItemFoundFlag & static_cast<uint8>(EInventorySlot::ClassItemArmor)))
				{
					ItemFoundFlag |= static_cast<uint8>(EInventorySlot::ClassItemArmor);
					EquipmentManager->EquipInventoryItem(Item);
				}
				break;
			
		
			default:
				break;
		}


		if( ItemFoundFlag == 0xFF )
		{
			break;
		}
	}


	
}




