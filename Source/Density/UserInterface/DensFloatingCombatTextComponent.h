// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DensFloatingCombatTextComponent.generated.h"


class ADensFloatingTextActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DENSITY_API UDensFloatingCombatTextComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensFloatingCombatTextComponent();

	//Places text in specified location in world space
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "FloatingCombatTextComponent")
	void AddFloatingText_CLIENT(float Damage, bool bDisplayEmpowered, FVector WorldLocation);

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingCombatTextComponent")
	TSubclassOf<ADensFloatingTextActor> FloatingTextActorClass;

	
	UFUNCTION()
	void OnTextDestroyed(AActor* DestroyedActor);

	UPROPERTY()
	TArray<ADensFloatingTextActor*> SpawnedActors;
	
	
};
