// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DensStatusBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API UDensStatusBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float Health);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCharacterName(const FText& Name);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowStatusBar();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideStatusBar();

};
