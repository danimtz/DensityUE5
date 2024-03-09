// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "EncounterManager.h"


// Sets default values
AEncounterManager::AEncounterManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AEncounterManager::WipeFireteam()
{
	
	OnWipeFireteam.Broadcast();
	
}



void AEncounterManager::WipeEncounter()
{
}


void AEncounterManager::RestartEncounter_Implementation()
{
	
}


void AEncounterManager::BeginEncounter_Implementation()
{
	OnEncounterStarted.Broadcast();
}

// Called when the game starts or when spawned
void AEncounterManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEncounterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

