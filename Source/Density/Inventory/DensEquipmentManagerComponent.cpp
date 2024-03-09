// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensEquipmentManagerComponent.h"

#include "Armor/DensArmorInfo.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"
#include "Player/DensPlayerController.h"
#include "Player/DensPlayerState.h"
#include "Weapons/DensWeaponInstance.h"


// Sets default values for this component's properties
UDensEquipmentManagerComponent::UDensEquipmentManagerComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	KineticWeapon = nullptr;
	EnergyWeapon = nullptr;
	HeavyWeapon = nullptr;
	HeadArmor = nullptr;
	ChestArmor = nullptr;
	ArmsArmor = nullptr;
	LegArmor = nullptr;
	ClassItemArmor = nullptr;
	
}

void UDensEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();



	
}

void UDensEquipmentManagerComponent::EquipInventoryItem(UDensInventoryItem* InvItem)
{

	EInventorySlot NewItemSlot = InvItem->GetInventorySlot();
	
	
	
	/*switch(NewItemSlot)
	{
		case EInventorySlot::HeadArmor:
		case EInventorySlot::ArmsArmor:
		case EInventorySlot::ChestArmor:
		case EInventorySlot::LegArmor:
		case EInventorySlot::ClassItemArmor:

			UDensArmorItemDefinition* NewArmor = Cast<UDensArmorItemDefinition>(InvItem->ItemInfo);
			//CodeHere
			break;

		case EInventorySlot::KineticWeapon:
		case EInventorySlot::EnergyWeapon:
		case EInventorySlot::HeavyWeapon:
			UDensWeaponItemDefinition* NewWeapon = Cast<UDensWeaponItemDefinition>(InvItem->ItemInfo);
			//CodeHere
			break;
		
	}*/

	//If Weapon or Armor
	if (static_cast<uint8>(NewItemSlot) <= 0b111)
	{
		//UDensWeaponItemDefinition* NewWeapon = Cast<UDensWeaponItemDefinition>(InvItem->ItemInfo);
		EquipWeapon(InvItem);
	}
	else
	{
		//UDensArmorItemDefinition* NewArmor = Cast<UDensArmorItemDefinition>(InvItem->ItemInfo);
		EquipArmor(InvItem);
	}
}







void UDensEquipmentManagerComponent::EquipArmor(UDensInventoryItem* NewArmor)
{



	//Assign ItemDefinition to equipped slot and mark bitflag dirty
	switch (NewArmor->GetInventorySlot())
	{
		case EInventorySlot::HeadArmor:
			if(HeadArmor)
			{
				HeadArmor->OnItemUnequipped();
			}
			HeadArmor = NewArmor;
			ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::HeadArmor);
			break;
			
		case EInventorySlot::ArmsArmor:
			if(ArmsArmor)
			{
				ArmsArmor->OnItemUnequipped();
			}
			ArmsArmor = NewArmor;
			ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::ArmsArmor);
			break;
			
		case EInventorySlot::ChestArmor:
			if(ChestArmor)
			{
				ChestArmor->OnItemUnequipped();
			}
			ChestArmor = NewArmor;
			ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::ChestArmor);
			break;
			
		case EInventorySlot::LegArmor:
			if(LegArmor)
			{
				LegArmor->OnItemUnequipped();
			}
			LegArmor = NewArmor;
			ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::LegArmor);
			break;
			
		case EInventorySlot::ClassItemArmor:
			if(ClassItemArmor)
			{
				ClassItemArmor->OnItemUnequipped();
			}
			ClassItemArmor = NewArmor;
			ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::ClassItemArmor);
			break;
			
		default:
			UE_LOG(LogTemp, Error, TEXT("DensEquipmentManagerComponent: ArmorItemDefinition EInventorySlot was missing or not valid for [%s]"), *GetNameSafe(NewArmor->ItemInfo));
			break;
	
	}


		//TODO Add ability set armor stats and abilities
	
	
	UpdatePlayerEquipment();



	
}



void UDensEquipmentManagerComponent::EquipWeapon(UDensInventoryItem* NewWeapon)
{


	switch (NewWeapon->GetInventorySlot())
	{
	case EInventorySlot::KineticWeapon:
		if(KineticWeapon)
		{
			KineticWeapon->OnItemUnequipped();
		}
		KineticWeapon = NewWeapon;
		ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::KineticWeapon);
		break;
	
	case EInventorySlot::EnergyWeapon:
		if(EnergyWeapon)
		{
			EnergyWeapon->OnItemUnequipped();
		}
		EnergyWeapon = NewWeapon;
		ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::EnergyWeapon);
		break;
	
	case EInventorySlot::HeavyWeapon:
		if(HeavyWeapon)
		{
			HeavyWeapon->OnItemUnequipped();
		}
		HeavyWeapon = NewWeapon;
		ItemDirtyFlag |= static_cast<uint8>(EInventorySlot::HeavyWeapon);
		break;
	
		
	
	default:
		UE_LOG(LogTemp, Error, TEXT("DensEquipmentManagerComponent: WeaponItemDefinition EInventorySlot was missing or not valid for [%s]"), *GetNameSafe(NewWeapon->ItemInfo));
		break;
	}


	UpdatePlayerEquipment();
	
}




void UDensEquipmentManagerComponent::UpdatePlayerEquipment()
{

	if(ItemDirtyFlag != 0)
	{

		ADensPlayerState* DensPS = Cast<ADensPlayerState>(GetOuter()); //GetOuter<ADensPlayerState>();
		ADensCharacter* DensCharacter = Cast<ADensCharacter>(DensPS->GetPlayerController()->GetCharacter());
		

		if(DensCharacter)
		{
			UDensWeaponEquipmentComponent* WeaponEC = DensCharacter->GetWeaponEquipmentComponent();
			if(WeaponEC)
			{
					
				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::KineticWeapon))
				{
					KineticWeapon->OnItemEquipped();
					WeaponEC->InitNewWeapon(KineticWeapon->ItemInfo->GetWeaponInstance());

				}

				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::EnergyWeapon))
				{
					EnergyWeapon->OnItemEquipped();
					WeaponEC->InitNewWeapon(EnergyWeapon->ItemInfo->GetWeaponInstance());

				}


				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::HeavyWeapon))
				{
					HeavyWeapon->OnItemEquipped();
					WeaponEC->InitNewWeapon(HeavyWeapon->ItemInfo->GetWeaponInstance());

				}

				
				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::HeadArmor))
				{
					HeadArmor->OnItemEquipped();
					DensCharacter->SetHeadMesh(HeadArmor->ItemInfo->GetArmorInfo()->ArmorMesh);

				}


				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::ArmsArmor))
				{
					ArmsArmor->OnItemEquipped();
					DensCharacter->SetArmsMesh(ArmsArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
					DensCharacter->SetArmsFPMesh(ArmsArmor->ItemInfo->GetArmorInfo()->ArmorMesh_FirstPerson);

				}


				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::ChestArmor))
				{
					ChestArmor->OnItemEquipped();
					DensCharacter->SetChestMesh(ChestArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
					DensCharacter->SetChestFPMesh(ChestArmor->ItemInfo->GetArmorInfo()->ArmorMesh_FirstPerson);

				}


				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::LegArmor))
				{
					LegArmor->OnItemEquipped();
					DensCharacter->SetLegsMesh(LegArmor->ItemInfo->GetArmorInfo()->ArmorMesh);

				}

				
				
				if(ItemDirtyFlag & static_cast<uint8>(EInventorySlot::ClassItemArmor))
				{
					ClassItemArmor->OnItemEquipped();
					DensCharacter->SetClassItemMesh(ClassItemArmor->ItemInfo->GetArmorInfo()->ArmorMesh);

				}


				ItemDirtyFlag = 0;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerCharacter not found in UDensEquipmentManagerComponent function %s"), *FString(__FUNCTION__));
			
		}
	}
	
}

void UDensEquipmentManagerComponent::RefreshPlayerEquipment()
{
	
	ADensPlayerState* DensPS = Cast<ADensPlayerState>(GetOuter()); //GetOuter<ADensPlayerState>();
	if(!DensPS)
	{
		return;
	}

	ADensPlayerController* DensPC = Cast<ADensPlayerController>(DensPS->GetPlayerController());
	if(!DensPC)
	{
		return;
	}
	
	ADensCharacter* DensCharacter = Cast<ADensCharacter>(DensPC->GetCharacter());
	
	if(DensCharacter)
	{
		UDensWeaponEquipmentComponent* WeaponEC = DensCharacter->GetWeaponEquipmentComponent();
		if(WeaponEC)
		{
			if(KineticWeapon)
			{
				if(KineticWeapon->bIsEquipped)
				{
					WeaponEC->InitNewWeapon(KineticWeapon->ItemInfo->GetWeaponInstance(), true);
				}
			}
			
			
		
			if(EnergyWeapon)
			{
				if(EnergyWeapon->bIsEquipped)
				{
					WeaponEC->InitNewWeapon(EnergyWeapon->ItemInfo->GetWeaponInstance(), true);
				}
			}
		

			if(HeavyWeapon)
			{
				if(HeavyWeapon->bIsEquipped)
				{
					WeaponEC->InitNewWeapon(HeavyWeapon->ItemInfo->GetWeaponInstance(), true);
				}
			}
		
			if(HeadArmor)
			{
				if(HeadArmor->bIsEquipped)
				{
					DensCharacter->SetHeadMesh(HeadArmor->ItemInfo->GetArmorInfo()->ArmorMesh);

				}
			}

			
			if(ArmsArmor)
			{
				if(ArmsArmor->bIsEquipped)
				{
					DensCharacter->SetArmsMesh(ArmsArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
					DensCharacter->SetArmsFPMesh(ArmsArmor->ItemInfo->GetArmorInfo()->ArmorMesh_FirstPerson);
				}
			}

			
			if(ChestArmor)
			{
				if(ChestArmor->bIsEquipped)
				{
					DensCharacter->SetChestMesh(ChestArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
					DensCharacter->SetChestFPMesh(ChestArmor->ItemInfo->GetArmorInfo()->ArmorMesh_FirstPerson);
				}
			}
				
			
			if(LegArmor)
			{
				if(LegArmor->bIsEquipped)
				{
					DensCharacter->SetLegsMesh(LegArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
				}
			}

			
			if(ClassItemArmor)
			{
				if(ClassItemArmor->bIsEquipped)
				{
					DensCharacter->SetClassItemMesh(ClassItemArmor->ItemInfo->GetArmorInfo()->ArmorMesh);
				}
			}
			
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter not found in UDensEquipmentManagerComponent function %s"), *FString(__FUNCTION__));
		
	}

	
}

void UDensEquipmentManagerComponent::Reset()
{
	ItemDirtyFlag = 0xFF;
}

/*
void UDensEquipmentManagerComponent::OnReset()
{
	
}
*/
