// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensStatusBarComponent.h"

#include "DensStatusBarWidget.h"


// Sets default values for this component's properties
UDensStatusBarComponent::UDensStatusBarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
	
	bIsInitialized = false;

	SetWindowFocusable(false);
	
	
}

void UDensStatusBarComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	
}

void UDensStatusBarComponent::InitializeStatusBar(const TSubclassOf<UDensStatusBarWidget>& InWidgetClass, const FText& Name, float HealthPercentage)
{
	
	SetWidgetClass(InWidgetClass);
	StatusBarWidget = Cast<UDensStatusBarWidget>(GetWidget());
	if(!StatusBarWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() StatusBarWidgetClass not set."), *FString(__FUNCTION__));
	}

	
	SetHealthPercentage(HealthPercentage);
	SetName(Name);

	HideStatusBar();
	
	bIsInitialized = true;
}




// Called when the game starts
void UDensStatusBarComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}





void UDensStatusBarComponent::SetName(const FText& Text)
{
	if(StatusBarWidget)
	{
		StatusBarWidget->SetCharacterName(Text);
	}
}


void UDensStatusBarComponent::SetHealthPercentage(float HealthPercentage)
{
	if(StatusBarWidget)
	{
		StatusBarWidget->SetHealthPercentage(HealthPercentage);
	}
}

void UDensStatusBarComponent::ShowStatusBar()
{
	if(StatusBarWidget)
	{
		StatusBarWidget->ShowStatusBar();
	}
}

void UDensStatusBarComponent::HideStatusBar()
{
	if(StatusBarWidget)
	{
		StatusBarWidget->HideStatusBar();
	}
}



