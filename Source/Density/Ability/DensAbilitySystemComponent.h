// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.
// Some things from Den Kestranek GASShooter
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"


#include "DensAbilitySystemComponent.generated.h"


struct FDensDamageContainerSpec;


/**
* Data about montages that were played locally (all montages in case of server. predictive montages in case of client). Never replicated directly.
*/
USTRUCT()
struct DENSITY_API FGameplayAbilityLocalAnimMontageForMesh
{
	GENERATED_BODY();

public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;
	
	UPROPERTY()
	FGameplayAbilityLocalAnimMontage LocalMontageInfo;
	
	FGameplayAbilityLocalAnimMontageForMesh() : Mesh(nullptr), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh, FGameplayAbilityLocalAnimMontage& InLocalMontageInfo)
		: Mesh(InMesh), LocalMontageInfo(InLocalMontageInfo)
	{
	}
	
};


/**
* Data about montages that is replicated to simulated clients.
*/
USTRUCT()
struct DENSITY_API FGameplayAbilityRepAnimMontageForMesh
{
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY()
	FGameplayAbilityRepAnimMontage RepMontageInfo;

	FGameplayAbilityRepAnimMontageForMesh() : Mesh(nullptr), RepMontageInfo()
	{
	}

	FGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh) : Mesh(InMesh), RepMontageInfo()
	{
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};


template<>
struct TStructOpsTypeTraits<FGameplayAbilityRepAnimMontageForMesh> : public TStructOpsTypeTraitsBase2<FGameplayAbilityRepAnimMontageForMesh>
{
	enum
	{
		WithNetSerializer = true,
	};
};

UCLASS()
class DENSITY_API UDensAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	

public:

	UDensAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool GetShouldTick() const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	
	static UDensAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass);

	
	// Turn on RPC batching in ASC. Off by default.
	virtual bool ShouldDoServerAbilityRPCBatch() const override { return true; }

	// From GAS Shooter
	// Attempts to activate the given ability handle and batch all RPCs into one. This will only batch all RPCs that happen
	// in one frame. Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	// Worst case we batch ActivateAbility and SendTargetData into one RPC instead of two and call EndAbility later in a separate
	// RPC. If we can't batch SendTargetData or EndAbility with ActivateAbility because they don't happen in the same frame due to
	// latent ability tasks for example, then batching doesn't help and we should just activate normally.
	// Single shots (semi auto fire) combine ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	// Full auto shots combine ActivateAbility and SendTargetData into one RPC instead of two for the first bullet. Each subsequent
	// bullet is one RPC for SendTargetData. We then send one final RPC for the EndAbility when we're done firing.
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);



	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocalOnOwningClient(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	
	// ----------------------------------------------------------------------------------------------------------------
	//	AnimMontage Support for multiple USkeletalMeshComponents on the AvatarActor.
	//  From GASShooter
	// ----------------------------------------------------------------------------------------------------------------	

	// Plays a montage and handles replication and prediction based on passed in ability/activation info
	virtual float PlayMontageForMesh(UGameplayAbility* AnimatingAbility, class USkeletalMeshComponent* InMesh, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, bool bReplicateMontage = true);

	// Plays a montage without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a montage.
	virtual float PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None);

	// Stops whatever montage is currently playing. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	virtual void CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	// Stops all montages currently playing
	virtual void StopAllCurrentMontages(float OverrideBlendOutTime = -1.0f);

	// Stops current montage if it's the one given as the Montage param
	virtual void StopMontageIfCurrentForMesh(USkeletalMeshComponent* InMesh, const UAnimMontage& Montage, float OverrideBlendOutTime = -1.0f);

	// Clear the animating ability that is passed in, if it's still currently animating
	virtual void ClearAnimatingAbilityForAllMeshes(UGameplayAbility* Ability);

	// Jumps current montage to given section. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	virtual void CurrentMontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName);

	// Sets current montages next section name. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	virtual void CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	// Sets current montage's play rate
	virtual void CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, float InPlayRate);

	// Returns true if the passed in ability is the current animating ability
	bool IsAnimatingAbilityForAnyMesh(UGameplayAbility* Ability) const;

	// Returns the current animating ability
	UGameplayAbility* GetAnimatingAbilityFromAnyMesh();

	// Returns montages that are currently playing
	TArray<UAnimMontage*> GetCurrentMontages() const;

	// Returns the montage that is playing for the mesh
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	// Get SectionID of currently playing AnimMontage
	int32 GetCurrentMontageSectionIDForMesh(USkeletalMeshComponent* InMesh);

	// Get SectionName of currently playing AnimMontage
	FName GetCurrentMontageSectionNameForMesh(USkeletalMeshComponent* InMesh);

	// Get length in time of current section
	float GetCurrentMontageSectionLengthForMesh(USkeletalMeshComponent* InMesh);

	// Returns amount of time left in current section
	float GetCurrentMontageSectionTimeLeftForMesh(USkeletalMeshComponent* InMesh);

	//Removes meshes from array (used when despawning some actors such as weapons)
	void RemoveMeshFromAnimMontageInfoForMeshes(USkeletalMeshComponent* InMesh);


	// ----------------------------------------------------------------------------------------------------------------
	//	Damage pipeline functions
	// ----------------------------------------------------------------------------------------------------------------	
	
	//Applies the given damage container spec from the current ASC. Executes damage pipeline
	UFUNCTION(BlueprintCallable, Category = "DamagePipeline")
	void ApplyDamageContainerSpec(const FDensDamageContainerSpec& ContainerSpec);

	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, int32 NewLevel);


	
protected:

	
	//ASC overrides. This makes Wait input released and pressed work
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	//TEMP fix for gameplay cues until UE5.4
	virtual void RemoveGameplayCue_Internal(const FGameplayTag GameplayCueTag, FActiveGameplayCueContainer& GameplayCueContainer) override;


	
	// ----------------------------------------------------------------------------------------------------------------
	//	AnimMontage Support for multiple USkeletalMeshComponents on the AvatarActor.
	// ----------------------------------------------------------------------------------------------------------------	

	// Set if montage rep happens while we don't have the animinstance associated with us yet
	UPROPERTY()
	bool bPendingMontageRepForMesh;

	// Data structure for montages that were instigated locally (everything if server, predictive if client. replicated if simulated proxy)
	// Will be max one element per skeletal mesh on the AvatarActor
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontageForMesh> LocalAnimMontageInfoForMeshes;

	// Data structure for replicating montage info to simulated clients
	// Will be max one element per skeletal mesh on the AvatarActor
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimMontageForMesh)
	TArray<FGameplayAbilityRepAnimMontageForMesh> RepAnimMontageInfoForMeshes;

	// Finds the existing FGameplayAbilityLocalAnimMontageForMesh for the mesh or creates one if it doesn't exist
	FGameplayAbilityLocalAnimMontageForMesh& GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);
	// Finds the existing FGameplayAbilityRepAnimMontageForMesh for the mesh or creates one if it doesn't exist
	FGameplayAbilityRepAnimMontageForMesh& GetGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh);

	// Called when a prediction key that played a montage is rejected
	void OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage);

	// Copy LocalAnimMontageInfo into RepAnimMontageInfo
	void AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh);
	void AnimMontage_UpdateReplicatedDataForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo);

	// Copy over playing flags for duplicate animation data
	void AnimMontage_UpdateForcedPlayFlagsForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo);	

	UFUNCTION()
	virtual void OnRep_ReplicatedAnimMontageForMesh();

	// Returns true if we are ready to handle replicated montage information
	virtual bool IsReadyForReplicatedMontageForMesh();

	// RPC function called from CurrentMontageSetNextSectionName, replicates to other clients
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	void ServerCurrentMontageSetNextSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	bool ServerCurrentMontageSetNextSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);

	// RPC function called from CurrentMontageJumpToSection, replicates to other clients
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCurrentMontageJumpToSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	void ServerCurrentMontageJumpToSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	bool ServerCurrentMontageJumpToSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);

	// RPC function called from CurrentMontageSetPlayRate, replicates to other clients
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	void ServerCurrentMontageSetPlayRateForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	bool ServerCurrentMontageSetPlayRateForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);


	
protected:

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;


	
};
