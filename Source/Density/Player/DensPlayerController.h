// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DensPlayerController.generated.h"


class UDensInventoryManagerComponent;
class ADensCharacterBase;
class UDensAbilitySystemComponent;
class UDensFloatingCombatTextComponent;
class UDensHUDWidget;
class UDensCombatReticleWidget;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDiedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRespawnDelegate, ADensCharacter*, Character);

/**
 * 
 */
UCLASS()
class DENSITY_API ADensPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:

	ADensPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerController")
	ADensPlayerState* GetDensPlayerState() const;


	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerController")
	UDensAbilitySystemComponent* GetDensAbilitySystemComponent() const;

	
	void InitHUD();

	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerController|UI")
	UDensHUDWidget* GetMainHUD();

	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerController|UI")
	UDensCombatReticleWidget* GetReticleHUD();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnHUDInitialized();
	

	
	UFUNCTION(BlueprintImplementableEvent)
	void BeginIncomingWipeHUDWidget();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RemoveIncomingWipeHUDWidget();
	
	
	
	UFUNCTION()
	void ShowDamageNumber(float DamageAmount, AActor* TargetCharacter,  const FHitResult& HitResult, bool bDisplayEmpowered = false);
	
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamage(float DamageAmount,  const FHitResult& HitResult, bool IsCriticalHit );

	
	UFUNCTION(BlueprintCallable, Category = "Dens|PlayerController")
	UDensInventoryManagerComponent* GetInventoryManagerComponent();


	UPROPERTY(BlueprintAssignable, Category = "Density | Player")
	FPlayerDiedDelegate PlayerDiedDelegate;

	
	UPROPERTY(BlueprintAssignable, Category = "Density | Player")
	FPlayerRespawnDelegate PlayerRespawnDelegate;
	

	UFUNCTION( Client, Reliable )
	void ToggleIncomingWipeWidget(bool bWidgetActive);

	
	UFUNCTION( Client, Reliable )
	void OnPlayerDeath_CLIENT(FCharacterDeathInfo DeathInfo);

	UFUNCTION()
	void OnPlayerDeath();


public:

	UPROPERTY(BlueprintReadOnly, Category = "Density | Combat")
	FText DeathReason; //TODO Change this to a struct with death data
	
	
protected:
	

	//UI
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Density|UI")
	TSubclassOf<class UDensHUDWidget> HUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Density|UI")
	class UDensHUDWidget* HUDWidget;


	UPROPERTY(BlueprintReadWrite, Category = "Density|UI")
	class UDeathScreenWidget* DeathScreenWidget;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Density|UI")
	TSubclassOf<class UUserWidget> DeathScreenWidgetClass;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Density|UI")
	TSubclassOf<class UDensCombatReticleWidget> ReticleWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Density|UI")
	class UDensCombatReticleWidget* ReticleWidget;


	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UDensFloatingCombatTextComponent> CombatTextComponent;


	//Inventory
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UDensInventoryManagerComponent> InventoryManagerComponent;

	
	
	//~APlayerController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void OnRep_PlayerState() override;
	//~End of APlayerController interface


};
