// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DensAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UDensAssetManager();

	// Returns AssetManager singleton object
	static UDensAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;


};

