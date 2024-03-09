// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensFloatingCombatTextComponent.h"

#include "DensFloatingTextActor.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UDensFloatingCombatTextComponent::UDensFloatingCombatTextComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UDensFloatingCombatTextComponent::AddFloatingText_CLIENT_Implementation(float Damage, bool bDisplayEmpowered,
	FVector WorldLocation)
{
	if(FloatingTextActorClass == nullptr)
	{
		
		return;
	}

	//Spawn text actor at location
	const FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, WorldLocation);
	ADensFloatingTextActor* TextActor = GetWorld()->SpawnActorDeferred<ADensFloatingTextActor>(FloatingTextActorClass, SpawnTransform, GetOwner());
	if(TextActor == nullptr)
	{
		return;
	}

	TextActor->InitializeFloatingText(Damage, bDisplayEmpowered);
	TextActor->OnDestroyed.AddDynamic(this, &ThisClass::OnTextDestroyed);
	UGameplayStatics::FinishSpawningActor(TextActor, SpawnTransform);

	//Add new text actor to the start of the array
	SpawnedActors.Insert(TextActor, 0);
}


// Called when the game starts
void UDensFloatingCombatTextComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDensFloatingCombatTextComponent::OnTextDestroyed(AActor* DestroyedActor)
{
	SpawnedActors.Pop();
}


