// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DensPickupItem.generated.h"

class ADensCharacter;

UCLASS()
class DENSITY_API ADensPickupItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADensPickupItem();

	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "CanBePickedUp"))
	bool K2_CanBePickedUp(ADensCharacter* TargetCharacter) const;
	
	virtual bool K2_CanBePickedUp_Implementation(ADensCharacter* TargetCharacter) const;

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnItemPickedUp"))
	void K2_OnItemPickedUp(ADensCharacter* TargetCharacter) const;
	
protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	virtual bool CanPickUp(ADensCharacter* TargetCharacter) const;

	void PickUpItem(ADensCharacter* TargetCharacter);

	
	virtual void ApplyItemToCharacter(ADensCharacter* TargetCharacter);
	
};
