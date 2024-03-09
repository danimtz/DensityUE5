// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DensHelperFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensHelperFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	UFUNCTION(BlueprintCallable)
	static void SetGameFeatureActive(const FString& Name, bool bNewActive);
};
