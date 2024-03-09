// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "SquadSpawnDataComponent.h"

#include "Characters/Enemies/DensEnemy.h"


// Sets default values for this component's properties
USquadSpawnDataComponent::USquadSpawnDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//bIsEditorOnly = true;
	// ...


	
}


// Called when the game starts
void USquadSpawnDataComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	SetVisibility(false);
}




#if WITH_EDITOR
void USquadSpawnDataComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(USquadSpawnDataComponent, EnemyClass)) )
	{
		if(EnemyClass)
		{
			USkeletalMeshComponent* EnemyMesh = EnemyClass.GetDefaultObject()->GetMesh();
			if(EnemyMesh)
			{
				SetSkeletalMeshAsset(EnemyMesh->GetSkeletalMeshAsset());
			}
		}
	}

	
}
#endif

