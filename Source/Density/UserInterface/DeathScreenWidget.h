// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathScreenWidget.generated.h"

struct FCharacterDeathInfo;
/**
 * 
 */
UCLASS()
class DENSITY_API UDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Density")
	void DisplayDeathInfo(FCharacterDeathInfo DeathInfo);
	
	
	
};
