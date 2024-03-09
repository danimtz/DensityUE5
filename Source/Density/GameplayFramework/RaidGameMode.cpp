// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "RaidGameMode.h"

#include "Characters/DensCharacterBase.h"
#include "Characters/PlayerCharacters/DensCharacter.h"
#include "EncounterScripting/EncounterManager.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Player/DensPlayerController.h"


ARaidGameMode::ARaidGameMode()
	: Super()
{
	ActiveEncounterIndex = 0;
	WipeTimerDuration = 6.0f;
}



void ARaidGameMode::InitNextEncounter()
{

	if(ActiveEncounterIndex >= RaidEncounters.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("ActiveEncounterIndex higher than RaidEncounters in array in %s"),  *GetNameSafe(this));
		return; //FAIL
	}

	if(!RaidEncounters[ActiveEncounterIndex])
	{
		UE_LOG(LogTemp, Error, TEXT("RaidEncounter at index %d is null    in %s"), ActiveEncounterIndex, *GetNameSafe(this));
		return; //FAIL
	}

	

	//Destroy old encoutner manager
	if(ActiveEncounterManager)
	{
		ActiveEncounterManager->OnWipeFireteam.RemoveDynamic(this, &ThisClass::OnTeamWipe);
		ActiveEncounterManager = nullptr;
	}


	
	
	ActiveEncounterManager = RaidEncounters[ActiveEncounterIndex].Get();
	ActiveEncounterManager->OnWipeFireteam.AddUniqueDynamic(this, &ThisClass::OnTeamWipe);
	ActiveEncounterManager->OnEncounterStarted.AddUniqueDynamic(this, &ThisClass::EncounterStarted);
	ActiveEncounterManager->PreEncounterStart();
	
	ActiveEncounterIndex++;
}



void ARaidGameMode::OnTeamWipe()
{
	//TODO
	//Kills any remaining players, 
	for(auto PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		if(ADensCharacter* DensCharacter = Cast<ADensCharacter>(PlayerState->GetPawn()))
		{
			DensCharacter->Die();
		}
		
	}

	
	GetWorld()->GetTimerManager().SetTimer(
		WipeTimerHandle, // handle to cancel timer at a later time
		this, // the owning object
		&ARaidGameMode::WipeAndResetActiveEncounter, // function to call on elapsed
		WipeTimerDuration, // float delay until elapsed
		false); // looping?
	
}



void ARaidGameMode::WipeAndResetActiveEncounter()
{

	//Set player UI to wipe screen?

	
	//then resets encounter.
	
	


	bIsRespawningRestricted = false;
	GetWorld()->GetTimerManager().ClearTimer(WipeTimerHandle);
	//Reset players to whatever
	
	ResetLevel();

	ActiveEncounterManager->RestartEncounter();
	/*for(auto PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		
		PlayerState->GetPlayerController()->GetPawn()->Destroy();
	}*/
}


void ARaidGameMode::OnAllPlayersDead()
{
	Super::OnAllPlayersDead();

	if(bIsRespawningRestricted)
	{
		for( auto PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
		{
			ADensPlayerController* PC = Cast<ADensPlayerController>(PlayerState->GetOwner());
			if(PC)
			{
				PC->ToggleIncomingWipeWidget(true);
			}
		}
	

		//If wipe timer isnt active then begin wipe
		if(!GetWorld()->GetTimerManager().IsTimerActive(WipeTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(
			WipeTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&ARaidGameMode::WipeAndResetActiveEncounter, // function to call on elapsed
			WipeTimerDuration, // float delay until elapsed
			false); // looping?
		}
	}
	
}



void ARaidGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	ADensPlayerController* PC = Cast<ADensPlayerController>(NewPlayer);
	if(PC)
	{
		if(ActiveEncounterManager)
		{
			ActiveEncounterManager->OnPlayerRestart(PC);
		}
	}
	
}

void ARaidGameMode::EncounterStarted()
{
	bIsRespawningRestricted = true;
}
