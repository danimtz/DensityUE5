// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DensStatusBarComponent.generated.h"


class UDensStatusBarWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DENSITY_API UDensStatusBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDensStatusBarComponent();

	virtual void InitializeComponent() override;
	
	void InitializeStatusBar(const TSubclassOf<UDensStatusBarWidget>& InWidgetClass, const FText& Name, float HealthPercentage);
	

	UFUNCTION(BlueprintCallable, Category = "Density | Character | StatusBar")
	void SetName(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Density | Character | StatusBar")
	void SetHealthPercentage(float HealthPercentage);


	UFUNCTION(BlueprintCallable, Category = "Density | Character | StatusBar")
	void  ShowStatusBar();

	UFUNCTION(BlueprintCallable, Category = "Density | Character | StatusBar")
	void  HideStatusBar();


	bool IsInitialized() const { return bIsInitialized;};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


protected:

	
	TObjectPtr<UDensStatusBarWidget> StatusBarWidget;

	UPROPERTY(BlueprintReadWrite)
	bool bIsStatusBarActive;

	bool bIsInitialized;
};
