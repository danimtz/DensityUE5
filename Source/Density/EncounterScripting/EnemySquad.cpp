// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "EnemySquad.h"

#include "SquadSpawnDataComponent.h"
#include "Characters/Enemies/DensEnemy.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AEnemySquad::AEnemySquad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EnemyCount = 0;
	
	OnSquadDefeated.AddUniqueDynamic(this, &ThisClass::SquadDefeated);
	
}

TArray<ADensEnemy*> AEnemySquad::SpawnEnemies()
{
	//Only spawn enemies if on the server, replication will spawn them on clients
	if(!HasAuthority() || EnemyCount > 0 )
	{
		return SpawnedEnemies;
	}
	
	EnemyCount = EnemySpawnData.Num();

	for(auto Enemy : EnemySpawnData)
	{
		if(!Enemy.EnemyClass)
		{
			EnemyCount--;
			continue;
		}

		if(Enemy.SpawnDelaySeconds > 0.0f)
		{
			FTimerHandle SpawnTimer;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "SpawnEnemy", Enemy);
			GetWorld()->GetTimerManager().SetTimer(SpawnTimer, Delegate, Enemy.SpawnDelaySeconds, false);
		}
		else
		{
			SpawnEnemy(Enemy);	
		}
	}

	return SpawnedEnemies;
}





void AEnemySquad::SpawnEnemy(FEnemySpawnData& SpawnData)
{
	
	ADensEnemy* SpawnedEnemy = GetWorld()->SpawnActorDeferred<ADensEnemy>(*SpawnData.EnemyClass, SpawnData.SpawnLocation, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	
	SpawnedEnemy->OnCharacterDied.AddUniqueDynamic(this, &ThisClass::OnSquadEnemyDefeated);
	
	SpawnedEnemy->FinishSpawning(SpawnData.SpawnLocation, /*bIsDefaultTransform=*/ false);

	SpawnedEnemies.Add(SpawnedEnemy);
	//SpawnedEnemy->OnCharacterDied.AddUniqueDynamic(this, &ThisClass::OnSquadEnemyDefeated);
	
}

void AEnemySquad::OnSquadEnemyDefeated(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo)
{
	EnemyCount--;

	if(EnemyCount <= 0)
	{
		OnSquadDefeated.Broadcast(); //THIS SHOULD BE A DELEGATE

		SpawnedEnemies.Empty();
	}
}

void AEnemySquad::Reset()
{
	Super::Reset();


	for(ADensEnemy* Enemy : SpawnedEnemies)
	{
		if(Enemy)
		{
			Enemy->Destroy();
		}
	}
	
	SpawnedEnemies.Empty();

	EnemyCount = 0;
}

// Called when the game starts or when spawned
void AEnemySquad::BeginPlay()
{
	Super::BeginPlay();
	
}


void AEnemySquad::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	//Your Logic Here

	EnemySpawnData.Reset();
	for(auto Component : GetComponents())
	{
		if(USquadSpawnDataComponent* SquadData = Cast<USquadSpawnDataComponent>(Component))
		{
			FEnemySpawnData SpawnDataEntry;

			SpawnDataEntry.EnemyClass = SquadData->EnemyClass;
			//SpawnDataEntry.EnemyData = SquadData->EnemyData;
			float HeightAdjust = 0.0f;
			if(SquadData->EnemyClass)
			{
				HeightAdjust = SquadData->EnemyClass.GetDefaultObject()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			}
			
			SpawnDataEntry.SpawnLocation = SquadData->GetComponentTransform();
			SpawnDataEntry.SpawnLocation.AddToTranslation(FVector(0.0f, 0.0f, HeightAdjust));
			SpawnDataEntry.SpawnDelaySeconds = SquadData->SpawnDelaySeconds;

			//Offset yaw by 90
			FRotator Rotation = SquadData->GetComponentRotation();
			Rotation.Add(0, 90.0f, 0 );
			SpawnDataEntry.SpawnLocation.SetRotation(Rotation.Quaternion());
			
			EnemySpawnData.Emplace(SpawnDataEntry);

			
		}
	}
}



