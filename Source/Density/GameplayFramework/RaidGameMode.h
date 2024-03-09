// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DensityGameModeBase.h"
#include "RaidGameMode.generated.h"


class AEncounterManager;

/**
 * 
 */
UCLASS()
class DENSITY_API ARaidGameMode : public ADensityGameModeBase
{
	GENERATED_BODY()

public:
	
	ARaidGameMode();


	UFUNCTION(BlueprintCallable, Category= "Denstiy | RaidGamemode")
	void InitNextEncounter();

	

	//Kills any remaining players, then resets encounter after 4 seconds. (Cannot be cancelled)
	UFUNCTION()
	void OnTeamWipe();

	
	UFUNCTION()
	void WipeAndResetActiveEncounter();


	/*UFUNCTION(BlueprintImplementableEvent)
	void TeamWiping();
	*/

	
	virtual void OnAllPlayersDead() override;
	


	
	//UFUNCTION()
	//void OnEncounterCompleted();


	virtual void RestartPlayer(AController* NewPlayer) override;
	
public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AEncounterManager> ActiveEncounterManager;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSoftObjectPtr<AEncounterManager>> RaidEncounters;

	int ActiveEncounterIndex;

	UPROPERTY(BlueprintReadWrite, Category = "Density")
	float WipeTimerDuration;

protected:
	UFUNCTION()
	void EncounterStarted();
	
protected:
	
	FTimerHandle WipeTimerHandle;
};
