// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EncounterManager.generated.h"


class ADensPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWipeFireteamDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEncounterStartedDelegate);
//Handles the encounter logic. Should only exist on server and is directly handled by the gamemode
UCLASS()
class DENSITY_API AEncounterManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEncounterManager();


	
	UPROPERTY(BlueprintAssignable, Category = "Density | Encounter")
	FWipeFireteamDelegate OnWipeFireteam;

	UPROPERTY(BlueprintAssignable, Category = "Density | Encounter")
	FEncounterStartedDelegate OnEncounterStarted;
	
	UFUNCTION(BlueprintCallable, Category= "Density | Encounter")
	void WipeFireteam();


	UFUNCTION(BlueprintCallable, Category= "Density | Encounter")
	void WipeEncounter();



	UFUNCTION(BlueprintNativeEvent, Category= "Density | Encounter")
	void RestartEncounter();


	UFUNCTION(BlueprintImplementableEvent, Category= "Density | Encounter")
	void PreEncounterStart();


	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category= "Density | Encounter")
	void BeginEncounter();


	UFUNCTION(BlueprintImplementableEvent, Category= "Density | Encounter")
	void OnPlayerRestart(ADensPlayerController* PlayerController);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
