// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DensWeapon.generated.h"


class UDensWeaponData;
class UDensAbilitySystemComponent;



UCLASS()
class DENSITY_API ADensWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADensWeapon();



	/*UFUNCTION(Category = Weapon)
	FTransform GetWeaponADSOffset() const {return WeaponADSOffset; };

	UFUNCTION(Category = Weapon)
	void SetWeaponADSOffset(const FTransform& NewTransform);*/

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnWeaponFired();
	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnEnterADS();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnExitADS();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetUsesFirstPersonMaterial(bool bUsesFirstPerson);
	

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };

	void SetWeaponPerspective(bool bIsFirstPerson) {bIsFirstPersonWeapon =  bIsFirstPerson;};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
protected:

	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Category = Weapon, EditDefaultsOnly, BlueprintReadOnly)
	FTransform WeaponADSOffset;

	UPROPERTY(Category = WeaponMetadata, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsFirstPersonWeapon;
	



	
};
