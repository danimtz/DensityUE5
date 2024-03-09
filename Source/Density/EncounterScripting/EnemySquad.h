// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySquad.generated.h"


class UDensEnemyData;
class ADensEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSquadDefeatedDelegate);



USTRUCT(BlueprintType)
struct FEnemySpawnData
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FTransform SpawnLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ADensEnemy> EnemyClass;

	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnDelaySeconds;
};




UCLASS()
class DENSITY_API AEnemySquad : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySquad();

	UFUNCTION(BlueprintCallable)
	TArray<ADensEnemy*> SpawnEnemies();

	UFUNCTION(BlueprintImplementableEvent)
	void SquadDefeated();
	
	//UFUNCTION()
	//void OnSquadMemberDefeated();
	
	virtual void Reset() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void SpawnEnemy(FEnemySpawnData& SpawnData);

	UFUNCTION()
	void OnSquadEnemyDefeated(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo);

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Density")
	FSquadDefeatedDelegate OnSquadDefeated;

	
protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Density")
	TArray<FEnemySpawnData> EnemySpawnData;


	//Used to keep reference to spawned enemies. Emptied when reseting level or when all squad defeated
	TArray<ADensEnemy*> SpawnedEnemies;

	int EnemyCount;
	
};


