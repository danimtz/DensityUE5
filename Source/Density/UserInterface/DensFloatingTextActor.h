// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DensFloatingTextActor.generated.h"

class UWidgetComponent;
class UDensFloatingTextWidget;

UCLASS()
class DENSITY_API ADensFloatingTextActor : public AActor
{
	GENERATED_BODY()

public:
	ADensFloatingTextActor();


	UFUNCTION(BlueprintImplementableEvent)
	void InitializeFloatingText(float Damage, bool bDisplayEmpowered);

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Density|UI")
	TSubclassOf<UDensFloatingTextWidget> FloatingTextWidgetClass;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Denstiy|UI")
	UWidgetComponent* FloatingTextWidgetComponent;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Denstiy|UI")
	class UDensFloatingTextWidget* FloatingTextWidget;
};
