// Copyright Epic Games, Inc. All Rights Reserved.


#include "DensityGameModeBase.h"

#include "Player/DensPlayerState.h"
#include "Player/DensPlayerController.h"

ADensityGameModeBase::ADensityGameModeBase()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerStateClass = ADensPlayerState::StaticClass();
	PlayerControllerClass = ADensPlayerController::StaticClass();

	PlayersAliveCount = 0;
}


void ADensityGameModeBase::OnPlayerAliveCountChanged_Implementation(int PlayersAlive)
{
	if(PlayersAlive <= 0)
	{
		OnAllPlayersDead();
	}
	
}



void ADensityGameModeBase::OnAllPlayersDead()
{
}



void ADensityGameModeBase::PlayerRespawned(ADensCharacter* Character)
{
	PlayersAliveCount = PlayersAliveCount+1;

	OnPlayerAliveCountChanged(PlayersAliveCount);
}


void ADensityGameModeBase::PlayerDied()
{
	PlayersAliveCount = PlayersAliveCount - 1;

	if(PlayersAliveCount<0)
	{
		PlayersAliveCount = 0;
	}

	OnPlayerAliveCountChanged(PlayersAliveCount);
}

void ADensityGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);


	if(ADensPlayerController* DensPlayerController = Cast<ADensPlayerController>(NewPlayer))
	{
		
		DensPlayerController->PlayerDiedDelegate.AddUniqueDynamic(this, &ThisClass::PlayerDied);
		DensPlayerController->PlayerRespawnDelegate.AddUniqueDynamic(this, &ThisClass::PlayerRespawned);
		
	}
	
}
