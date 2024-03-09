// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensFloatingTextActor.h"
#include "DensFloatingTextWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADensFloatingTextActor::ADensFloatingTextActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FloatingTextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("FloatingTextWidgetComponent"));
	FloatingTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	RootComponent = FloatingTextWidgetComponent;
}



// Called when the game starts or when spawned
void ADensFloatingTextActor::BeginPlay()
{
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->IsLocalPlayerController())
	{
		FloatingTextWidget = CreateWidget<UDensFloatingTextWidget>(PC, FloatingTextWidgetClass);
		if (FloatingTextWidget && FloatingTextWidgetComponent)
		{
			FloatingTextWidgetComponent->SetWidget(FloatingTextWidget);
		}
	}

	
	Super::BeginPlay();
}


