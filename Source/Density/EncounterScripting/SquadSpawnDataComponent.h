// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadSpawnDataComponent.generated.h"

class ADensEnemy;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), hidecategories=(SkeletalMesh))
class DENSITY_API USquadSpawnDataComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USquadSpawnDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Squad" )
	TSubclassOf<ADensEnemy> EnemyClass;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Squad")
	//UDensEnemyData* EnemyData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Squad")
	float SpawnDelaySeconds = 0.0f;

#if WITH_EDITOR
	// UObject interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	// End of UObject interface
#endif
	
	
	
};
