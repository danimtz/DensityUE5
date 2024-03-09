// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DensityGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DENSITY_API ADensityGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ADensityGameModeBase();
	


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int PlayersAliveCount;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool bIsRespawningRestricted;
	
protected:

	UFUNCTION(BlueprintNativeEvent)
	void OnPlayerAliveCountChanged(int PlayersAlive);

	
	virtual void OnAllPlayersDead();
	

	UFUNCTION()
	void PlayerRespawned(ADensCharacter* Character);

	UFUNCTION()
	void PlayerDied();
	
	virtual void OnPostLogin(AController* NewPlayer) override;


protected:

	
	
};
