// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DensInventoryManagerComponent.generated.h"


class UDensInventoryItem;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class DENSITY_API UDensInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensInventoryManagerComponent();

	virtual void InitializeComponent() override;


	//Called from player character OnPosses. TODO Maybe change this and how inventory works
	UFUNCTION()
	void InitInventory();
	
protected:

	void InitInventoryFromDataTable();

	
	
protected:

	bool bIsInventoryInnited;

	//TEMP For now inventory is initialized from data table
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> InventoryTable;

	
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UDensInventoryItem>> InventoryItems;
	


};
