// Copyright 2023 Daniel Martinez Amigo.
// CMC Tutorial Copyright (c) 2023 Kyle Lautenbach
#pragma once

#include "CoreMinimal.h"
#include "../../../../../../../Program Files (x86)/Windows Kits/10/include/10.0.22000.0/um/dbgeng.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DensCharacterMovementComponent.generated.h"



/////BEGIN Network Prediction Setup/////
#pragma region Network Prediction Setup



//Packed moved stuff

class FCustomNetworkMoveData : public FCharacterNetworkMoveData
{

public:

	typedef FCharacterNetworkMoveData Super;

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;

	//SAFE variables
	bool bWantsToSprintMoveData = false;
	bool bPressedForwardInputMoveData = false;
	
	//UNSAFE variables
	//float MaxCustomSpeedMoveData = 800.f;
	FVector LaunchVelocityCustomMoveData = FVector(0.f, 0.f, 0.f);

	//This bypasses the limitations of the typical compressed flags used in past versions of UE4. 
	//You would still use bitflags like this in games in order to improve network performance.
	//Imagine hundreds of clients sending this info every tick. Even a small saving can add up significantly, especially when considering server costs.
	//It is up to you to decide if you prefer sending bools like above or using the lightweight bitflag approach like below.
	//If you're making P2P games, casual online, or co-op vs AI, etc, then you might not care too much about maximising efficiency. The bool approach might be more readable.
	uint8 MovementFlagCustomMoveData = 0;
};


class FCustomCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{

public:

	FCustomCharacterNetworkMoveDataContainer();

	FCustomNetworkMoveData CustomDefaultMoveData[3];
};



//Saved move stuff

//Class FCustomSavedMove
class FCustomSavedMove : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;


	//All Saved Variables are placed here.
	//Boolean Flags
	bool bWantsToSprint_Saved = false;

	bool bPressedForwardInput_Saved = false;


	
	//Not present in Move Data. This state is not sent over the network, it is inferred from running the internal CMC logic.
	//However, we still save it for replay purposes.
	//bool bWallRunIsRightSaved = false;

	//As you can see, our bWantsToFly variable is not present in MoveData or here in SavedMove like bWantsToSprint is. We use the info from MovementFlagCustomMoveData to change our state and save it as SavedMovementFlagCustom.
	//This is because Move Data is sent back and forth, much like the Compressed Flags were sent in the old system (before packed move data).
	//Thus, we aim to minimise the number of variables in our Move Data for the sake of network performance.
	//We can avoid having this variable here at all as our SavedMovementFlagCustom and normal MovementFlagCustom are both already present.
	//bool bWantsToFlySaved = false; //It would have otherwise been stored here like this if we were using the bool approach.

	//Contains our saved custom movement flags, like CFLAG_WantsToFly.
	uint8 SavedMovementFlagCustom = 0;

	//Variables
	//float SavedMaxCustomSpeed = 800.f;
	FVector SavedLaunchVelocityCustom = FVector(0.f, 0.f, 0.f);


	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override;

	/** Returns true if this move can be combined with NewMove for replication without changing any behaviour.
	* Just determines if any variables were modified between moves for optimisation purposes. If nothing changed, combine moves to save time.
	*/
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
	virtual void PrepMoveFor(class ACharacter* Character) override;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;
};



//Class Prediction Data
class FCustomNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
{
public:
	FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};



#pragma endregion
/////END Network Prediction Setup/////





UENUM(BlueprintType)
enum ECustomMovementMode {
	MOVE_CustomNone UMETA(Hidden),
	MOVE_WallRunning   UMETA(DisplayName = "WallRunning"),
};

/*
*This uses bitshifting to pack a whole lot of extra flags into one tiny container, which can lower your bitrate usage.
* Sending larger data types via packed moves can negatively impact network performance.
* However, every project is different.You might prefer the readability and ease of use of using normal data types like bools(as demonstrated).
* But this may not cut it in a networked environment that requires minimal network usage(like saving on server bandwidth costs in AA to AAA games).
*/
UENUM(BlueprintType, Meta = (Bitflags))
enum class EMovementFlag : uint8
{
	NONE = 0 UMETA(Hidden), //Requires a 0 entry as default when initialised, but we hide it from BP.

	CFLAG_WantsToFly = 1 << 0,

	CFLAG_OtherFlag1 = 1 << 1, //This could be used as CFLAG_WantsToSprint and follow the same logic as CFLAG_WantsToFly. Instead, this tutorial demonstrates both approaches for you to decide your preference. But keep your project requirements in mind in regards to bitrate.
	CFLAG_OtherFlag2 = 1 << 2,
	CFLAG_OtherFlag3 = 1 << 3,
};




class ADensCharacter;


UCLASS()
class DENSITY_API UDensCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UDensCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	//Reference custom network prediction class
	friend class FCustomSavedMove;


	/////BEGIN Sprinting/////

	
	/*
	* We create two variables here. This one tracks player intent, such as holding the sprint button down or toggling it.
	* While the player intends to sprint, the movement can use this information to trigger different logic. We can even re-use it to act as an indicator to start wall-running, for instance.
	* In this case, if the player intends to sprint but isn't sprinting, we trigger sprinting.
	* Similarly, if the sprinting is interrupted, we would want to resume sprinting as soon as possible.
	* The implementation of this logic may differ in a GAS (Gameplay Ability System) setup where this variable will be controlled by Gameplay Abilities (GA) and Gameplay Effects (GE).
	* We do not replicate the variable as it's only really relevant to the owning client and the server.
	* The network prediction setup will ensure sync between owning client and server.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool bWantsToSprint;

	/*
	* This variable controls the actual sprinting logic. If it's true, the character will be moving at a higher velocity.
	* It can be used as an internal CMC variable to track a gameplay tag that is applied/removed by GAS (e.g. State.Movement.Sprinting or State.Buff.Sprinting, depending on preference and design).
	* But in this basic tutorial, we will use it directly.
	* This is replicated as we want this variable to propagate down to simulated proxies (other clients).
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Sprinting")
	bool bIsSprinting;

	

	/*
	* A simple function to determine if the character is able to sprint in its current state.
	* This function does not factor in external state information at the moment.
	* If used with GAS, this could be a helper function to determine if the Sprint Ability can be applied or not, alongside the use of gameplay tags and other checks.
	*/
	UFUNCTION(BlueprintCallable, Category = "Sprinting")
	virtual bool CanSprint() const;

	/////END Sprinting/////



	
	/////BEGIN Mantle///////
	bool bPressedForwardInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Mantle")
	bool bMantleInputPressed;
	
	UFUNCTION(BlueprintCallable, Category = "Mantle")
	virtual bool CanAttemptMantle() const;

	
	
	//////END Mantle//////



public:
	

	///// CHARACTER INPUT FUNCTIONS //////

	//These can only be called on client so its movement safe. Never modify movement safe variables on server. 
	//Movement safe being variables that are used in SavedMove for movement replication
	UFUNCTION(BlueprintCallable)
	void StartSprint();

	UFUNCTION(BlueprintCallable)
	void StopSprint();

	UFUNCTION(BlueprintCallable)
	void CancelSprintAbility();

	UFUNCTION(BlueprintCallable)
	void ForwardInputPressed();
	
	UFUNCTION(BlueprintCallable)
	void ForwardInputReleased();

	UFUNCTION(BlueprintCallable)
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable)
	void StartCrouch();

	UFUNCTION(BlueprintCallable)
	void StopCrouch();

	//Overriden from Character Movement Component, to not allow crouching while falling
	virtual bool CanCrouchInCurrentState() const override;
	
protected:
	/** Character movement component owner */
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<ADensCharacterBase> DensCharaterOwner;


	
	//Mantle
private:

	//bool TryMantle();
	//FVector GetMantleStartLocation(FHitResult FrontHit, FHitResult SurfaceHit, bool bTallMantle) const;

	
	//HelperFunctions
private:

	
public:

	virtual void BeginPlay() override;

	//BEGIN UMovementComponent Interface
	virtual float GetMaxSpeed() const override;

	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	/** Update the character state in PerformMovement after the position change. Some rotation updates happen after this. */
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	/** Consider this to be the final chance to change logic before the next tick. It can be useful to defer certain actions to the next tick. */
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	/** Override jump behaviour to help us create custom logic. */
	//virtual bool CanAttemptJump() const override;
	//virtual bool DoJump(bool bReplayingMoves) override;
	//END UMovementComponent Interface
	// 
	// 


	

protected:
	/** Called after MovementMode has changed. Base implementation performs special handling for starting certain modes, then notifies the CharacterOwner.
	*	We update it to become our central movement mode switching function. All enter/exit functions should be stored here or in SetMovementMode.
	*/
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void EnterWalking();
	virtual void ExitWalking();
	virtual void EnterFalling();
	virtual void ExitFalling();



	//GAS
	UFUNCTION()
	void OnAbilitySystemUninitialized(UDensAbilitySystemComponent* InASC);
	
public:

	//New Move Data Container
	FCustomCharacterNetworkMoveDataContainer MoveDataContainer;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;


	//Proxy replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	
};


