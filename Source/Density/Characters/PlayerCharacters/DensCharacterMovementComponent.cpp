// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"

//Let's include our custom character
#include "DensCharacter.h"
#include "Components/CapsuleComponent.h"

//Network types required for replication 
#include "Net/UnrealNetwork.h"
#include "UObject/CoreNetTypes.h"


//Required for canceling abilities of GAS
#include "DensCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "DensGameplayTags.h"
#include "Ability/DensAbilitySystemComponent.h"


UDensCharacterMovementComponent::UDensCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	SetIsReplicatedByDefault(true);

	SetNetworkMoveDataContainer(MoveDataContainer);
	
	NavAgentProps.bCanCrouch = true;

}

void UDensCharacterMovementComponent::BeginPlay()
{
	
	Super::BeginPlay();
	DensCharaterOwner = Cast<ADensCharacterBase>(PawnOwner);

	DensCharaterOwner->OnAbilitySystemUnInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemUninitialized);
	
}


void UDensCharacterMovementComponent::OnAbilitySystemUninitialized(UDensAbilitySystemComponent* DensASC)
{

	DensASC->RemoveLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);
	DensASC->RemoveReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);

	DensASC->RemoveLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);
	DensASC->RemoveReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);

	
}


#pragma region Overriden functionality (Crouch)

bool UDensCharacterMovementComponent::CanCrouchInCurrentState() const
{
	if (!CanEverCrouch())
	{
		return false;
	}

	return (IsFalling()/*TODO CHANGE to has been  falling for less than .2 seconds*/ || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();

}
#pragma endregion



//Sprinting and movement speed changes
#pragma region Sprinting + Custom Speed

float UDensCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Custom)
	{

		/*switch (CustomMovementMode)
		{
		//case MOVE_WallRunning:
			//return MaxWallRunSpeed;
		}*/
	}

	if(!DensCharaterOwner)
	{
		return 0.0f;
	}
	
	if (MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking )
	{
		if (bIsSprinting)
		{

			return DensCharaterOwner->GetSprintSpeed() * DensCharaterOwner->GetSprintSpeedMod();
		}
		else
		{
			return DensCharaterOwner->GetWalkSpeed() * DensCharaterOwner->GetWalkSpeedMod();
		}
	}

	if(MovementMode == MOVE_Falling)
	{
		UDensAbilitySystemComponent* DensASC = DensCharaterOwner->GetDensAbilitySystemComponent();
		if (!DensASC)
		{
			return DensCharaterOwner->GetWalkSpeed() * DensCharaterOwner->GetWalkSpeedMod();
		}

		
		if(DensASC->HasMatchingGameplayTag(DensGameplayTags::State_Movement_Sprint))
		{
			return DensCharaterOwner->GetSprintSpeed() * DensCharaterOwner->GetSprintSpeedMod();
		}
		else
		{
			return DensCharaterOwner->GetWalkSpeed() * DensCharaterOwner->GetWalkSpeedMod();
		}
	}
	
	return Super::GetMaxSpeed();
	

}




/*
* This function does not factor in external state information.
* If used with GAS, this could be a helper function to determine if the Sprint Ability can be applied or not, alongside the use of gameplay tags and other checks.
*/
bool UDensCharacterMovementComponent::CanSprint() const
{
	if (DensCharaterOwner && IsMovingOnGround() && bWantsToSprint) //Only sprint if on ground 
	{
		//Check if moving forward
		FVector Forward = CharacterOwner->GetActorForwardVector();
		
		FVector MoveDirection = Velocity.GetSafeNormal();

		float VelocityDot = FVector::DotProduct(Forward, MoveDirection); //Confirm we are moving forward so the player can't sprint sideways or backwards.

		//UE_LOG(LogTemp, Warning, TEXT("SprintSpeed = , %f  "), VelocityDot);

		//return VelocityDot >= 0.7f; //Slight lenience so that small changes don't rapidly toggle sprinting. This should be a variable, but it is hard-coded here for simplicity.
		return true;
	}
	return false;
}

#pragma endregion




//Mantling
#pragma region Mantle
bool UDensCharacterMovementComponent::CanAttemptMantle() const
{
	
	if(DensCharaterOwner && !IsMovingOnGround() && bMantleInputPressed)
	{
		return true;
	}

	return false;
	
}


#pragma endregion






#pragma region Movement Mode Switching

void UDensCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (!HasValidData())
	{
		return;
	}

	//We perform our custom logic first, then allow the Super (parent) function to handle the usual movement mode changes as it calls the character's OnMovementModeChanged. 
	//You can change the order depending on your needs in this particular case. Simply read through the parent function to see what works best for you.

	/*
	* This is purely a personal preference thing, but I like to ensure there is always a State Entry and State Exit function for my movement modes.
	* You can even write these functions for the default modes if you aren't satisfied with their base implementation.
	* It is INCREDIBLY important to channel logic like this through a single "handler" function. This pattern can be used for many other systems that handle state changes.
	* Why? Because you want to clean up a state BEFORE entering a new one. You want a defined logic flow to prevent weird bugs.
	*
	* Here's a use case:
	* You have a Parkour movement mode that requires the player's capsule collision to be turned off during the move.
	* When you ENTER the move, you need to perform certain logic that turns off collision, prepares the movement component with new variables, etc.
	* However, when you want to end the move and return to Walking or Falling, you obviously always want collision to be turned back on.
	* What happens if it is interrupted, or another bit of logic dictates that the player should now enter Falling or Flying or something?
	* If that designer/programmer just sets the movement mode directly, you're going to be left with no collision. Or worse, in a networked environment you might have complete variable desync if done incorrectly.
	* Therefore, you need to ensure that everyone REQUESTS a movement mode change using a particular function.
	* In this case, we use SetMovementMode, which will then always call this function.
	* Using SetMovementMode is NOT inherently network-replicated if triggered by a client, so adding a RequestMovementModeChange() function can be useful to flip-flop our network prediction flags (like bWantsToSprint) and perform other checks.
	* Thus, no matter what dynamic bit of gameplay code triggers a movement change, it will be caught by this function and properly EXIT a mode before ENTERing the next.
	* This design pattern is handy for many systems, as mentioned, but movement is one such place where it can be essential.
	*/



	
	//First, call exit code for the PREVIOUS movement mode.
	if (PreviousMovementMode == MOVE_Custom)
	{

		/*switch (PreviousCustomMode)
		{
		case MOVE_WallRunning:
			//ExitWallRun();
		}*/
	}
	else
	{
		/*switch (PreviousMovementMode)
		{
		case MOVE_Flying:
			//ExitFlying();
		}*/
	}


	if(PreviousMovementMode == MOVE_Walking)
	{
		ExitWalking();
	}

	if(PreviousMovementMode == MOVE_Falling)
	{
		ExitFalling();
	}
	
	//Next, call entry code for the NEW movement mode.
	if (MovementMode == MOVE_Custom)
	{

		/*switch (CustomMovementMode)
		{
		//case MOVE_WallRunning:
			//EnterWallRun();
		}*/
	}
	else
	{
		/*switch (MovementMode)
		{
		//case MOVE_Flying:
			//EnterFlying();
		}*/
	}

	
	if(MovementMode == MOVE_Walking)
	{
		EnterWalking();
	}

	if(MovementMode == MOVE_Falling)
	{
		EnterFalling();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}



void UDensCharacterMovementComponent::EnterWalking()
{
	if(!DensCharaterOwner)
	{
		return;
	}
	
	UAbilitySystemComponent* DensASC = DensCharaterOwner->GetAbilitySystemComponent();
	if (DensASC)
	{
		DensASC->AddLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);
		DensASC->AddReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);

		//DensASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Movement.TestGrounded"))));
		//DensASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Movement.TestGrounded"))));
	}
}


void UDensCharacterMovementComponent::ExitWalking()
{
	if(!DensCharaterOwner)
	{
		return;
	}
	
	UAbilitySystemComponent* DensASC = DensCharaterOwner->GetAbilitySystemComponent();
	if (DensASC)
	{
		//DensASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Movement.TestGrounded"))));
		//DensASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("State.Movement.TestGrounded"))));
 		DensASC->RemoveLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);
		DensASC->RemoveReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Grounded);
	}
}

void UDensCharacterMovementComponent::EnterFalling()
{
	if(!DensCharaterOwner)
	{
		return;
	}
	
	UAbilitySystemComponent* DensASC = DensCharaterOwner->GetAbilitySystemComponent();
	if (DensASC)
	{
		DensASC->AddLooseGameplayTag(DensGameplayTags::State_Movement_Airborne);
		DensASC->AddReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Airborne);
	}
}


void UDensCharacterMovementComponent::ExitFalling()
{
	if(!DensCharaterOwner)
	{
		return;
	}
	
	UAbilitySystemComponent* DensASC = DensCharaterOwner->GetAbilitySystemComponent();
	if (DensASC)
	{
		DensASC->RemoveLooseGameplayTag(DensGameplayTags::State_Movement_Airborne);
		DensASC->RemoveReplicatedLooseGameplayTag(DensGameplayTags::State_Movement_Airborne);
	}
}

/*
* A super useful function! This function is called prior to the movement update, making it a great place to initiate certain logic.
* We can start sprinting or begin wall running, for instance. Observe the Super (parent) function to see how it's currently used for crouch logic.
*/
void UDensCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	// Proxies get replicated state. We don't need to run this logic for them.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		//Sprinting
		if (CanSprint())
		{
			//If entering sprint
			if (!bIsSprinting)
			{
				DensCharaterOwner->SprintStarted();
			}

			bIsSprinting = true;
			
		}
		else
		{
			//If stopping sprint
			if (!bIsSprinting)
			{
				DensCharaterOwner->SprintStopped();
			}


			bIsSprinting = false;
		}


		//Mantle
		if(bPressedForwardInput)
		{
			bMantleInputPressed = true;
		}
		else
		{
			bMantleInputPressed = false;
		}

		
		// Wall Run
		/*if (IsFalling())
		{
			TryWallRun();
		}*/
	}
}


/*
* As you'd imagine, this function is called at the end of a movement update. Good place to add code or checks to clean up certain functionality.
* In the parent function, this performs one last check to see if the character should uncrouch.
*/
void UDensCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
}


//Called on tick, can be used for setting values and movement modes for next tick.
void UDensCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	/*
	* The code below could be done in UpdateCharacterStateBeforeMovement, but there's no perceptible difference for the player.
	* I included it here to demonstrate that this function can serve a similar purpose as UpdateCharacterStateBeforeMovement and UpdateCharacterStateAfterMovement.
	* However, it is always called AFTER UpdateCharacterStateAfterMovementand and is typically used to set up logic for the next tick in custom CMCs.
	*/
	if (CharacterOwner->GetLocalRole() > ROLE_SimulatedProxy)
	{
		/*if (IsFlagActive((uint8)EMovementFlag::CFLAG_WantsToFly)) //We typecast to uint8 due to how we declared this function. It accepts uint8, not EMovementFlag in C++. 
		{
			if (CanFly())
			{
				SetMovementMode(MOVE_Flying);
			}
		}
		else if ((!IsFlagActive((uint8)EMovementFlag::CFLAG_WantsToFly) || !CanFly()) && MovementMode == MOVE_Flying)
		{
			SetMovementMode(MOVE_Falling);
		}*/
	}

	/*
	* This is where the launch value will be set for the next tick. Both the client and server run this code, which is why it is important that our LaunchVelocityCustom is tracked in our net code.
	* However, it is an UNSAFE variable. We must sanity check this logic when the time comes to execute it, based on our game's mechanics.
	* We can do these checks in HandlePendingLaunch.
	*/

	if ((MovementMode != MOVE_None) && IsActive() && HasValidData())
	{
		//PendingLaunchVelocity = LaunchVelocityCustom;
		//LaunchVelocityCustom = FVector(0.f, 0.f, 0.f);
	}
}

#pragma endregion



#pragma region Character Inputs


void UDensCharacterMovementComponent::StartSprint()
{
	bWantsToSprint = true;
}

void UDensCharacterMovementComponent::StopSprint()
{
	bWantsToSprint = false;
}

void UDensCharacterMovementComponent::CancelSprintAbility()
{

	UAbilitySystemComponent* DensASC = DensCharaterOwner->GetAbilitySystemComponent();
	if (DensASC)
	{
		FGameplayTagContainer AbilityTypesToCancel;
		AbilityTypesToCancel.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Type.Action.Sprint")));
		DensASC->CancelAbilities(&AbilityTypesToCancel);
	}
	
}

void UDensCharacterMovementComponent::ForwardInputPressed()
{
	bPressedForwardInput = true;
}

void UDensCharacterMovementComponent::ForwardInputReleased()
{
	bPressedForwardInput = false;
}
/*
void UDensCharacterMovementComponent::ToggleSprint()
{

	bWantsToSprint = !bWantsToSprint;

}*/


void UDensCharacterMovementComponent::ToggleCrouch()
{
	
	if (bWantsToCrouch)
	{
		StopCrouch();
	}
	else if (!bWantsToCrouch && IsMovingOnGround())
	{
		StartCrouch();
	}
}

void UDensCharacterMovementComponent::StartCrouch()
{
	bWantsToCrouch = true;
	bWantsToSprint = false;
}

void UDensCharacterMovementComponent::StopCrouch()
{
	if (bWantsToCrouch)//Cancel sprint only if player was crouched
	{
		bWantsToSprint = false;
	}

	bWantsToCrouch = false;
}



#pragma endregion




#pragma region Replication (LifetimeReplicatedProps + OnReps)

//Standard replication function
void UDensCharacterMovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, bIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bMantleInputPressed, COND_SimulatedOnly);
	//DOREPLIFETIME_CONDITION(ThisClass, bWallRunIsRight, COND_SimulatedOnly);
}

//OnReps allow us to perform logic upon receiving a server update.
//Keep your on reps close to LifetimeRepProps in their own replication section, like this, so they can easily be tracked.


#pragma endregion



////BEGIN Networking////
#pragma region Networked Movement
/*
* General workflow adapted from SNM1 and SMN2 by Reddy-dev.
* The explanation below comes from SMN1.
* Copyright (c) 2021 Reddy-dev
*@Documentation Extending Saved Move Data
*
To add new data, first extend FSavedMove_Character to include whatever information your Character Movement Component needs. Next, extend FCharacterNetworkMoveData and add the custom data you want to send across the network; in most cases, this mirrors the data added to FSavedMove_Character. You will also need to extend FCharacterNetworkMoveDataContainer so that it can serialize your FCharacterNetworkMoveData for network transmission, and deserialize it upon receipt. When this setup is finished, configure the system as follows:
Modify your Character Movement Component to use the FCharacterNetworkMoveDataContainer subclass you created with the SetNetworkMoveDataContainer function. The simplest way to accomplish this is to add an instance of your FCharacterNetworkMoveDataContainer to your Character Movement Component child class, and call SetNetworkMoveDataContainer from the constructor.
Since your FCharacterNetworkMoveDataContainer needs its own instances of FCharacterNetworkMoveData, point it (typically in the constructor) to instances of your FCharacterNetworkMoveData subclass. See the base constructor for more details and an example.
In your extended version of FCharacterNetworkMoveData, override the ClientFillNetworkMoveData function to copy or compute data from the saved move. Override the Serialize function to read and write your data using an FArchive; this is the bit stream that RPCs require.
To extend the server response to clients, which can acknowledges a good move or send correction data, extend FCharacterMoveResponseData, FCharacterMoveResponseDataContainer, and override your Character Movement Component's version of the SetMoveResponseDataContainer.
*/

//Receives moves from Serialize

void UDensCharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	FCustomNetworkMoveData* CurrentMoveData = static_cast<FCustomNetworkMoveData*>(GetCurrentNetworkMoveData());
	if (CurrentMoveData != nullptr)
	{
		bWantsToSprint = CurrentMoveData->bWantsToSprintMoveData;
		bPressedForwardInput = CurrentMoveData->bPressedForwardInputMoveData;
		//If you still wanted to use bools AND bitflags, you could unpack movement flags like this.
		//It is similar to UpdateFromCompressedFlags in this sense. Check out that function in the parent to see how it's done.
		//EXAMPLE:
		//bWantsToFly = (CurrentMoveData->MovementFlagCustomMoveData & (uint8)EMovementFlag::CFLAG_WantsToFly) != 0;

		//LaunchVelocityCustom = CurrentMoveData->LaunchVelocityCustomMoveData;

		//MovementFlagCustom = CurrentMoveData->MovementFlagCustomMoveData;
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

//Sends the Movement Data 
bool FCustomNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	SerializeOptionalValue<bool>(Ar.IsSaving(), Ar, bWantsToSprintMoveData, false);
	SerializeOptionalValue<bool>(Ar.IsSaving(), Ar, bPressedForwardInputMoveData, false);
	SerializeOptionalValue<FVector>(Ar.IsSaving(), Ar, LaunchVelocityCustomMoveData, FVector(0.f, 0.f, 0.f));

	SerializeOptionalValue<uint8>(Ar.IsSaving(), Ar, MovementFlagCustomMoveData, 0);


	return !Ar.IsError();
}

void FCustomNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	Super::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FCustomSavedMove& CurrentSavedMove = static_cast<const FCustomSavedMove&>(ClientMove);

	bWantsToSprintMoveData = CurrentSavedMove.bWantsToSprint_Saved;
	bPressedForwardInputMoveData = CurrentSavedMove.bPressedForwardInput_Saved;
	LaunchVelocityCustomMoveData = CurrentSavedMove.SavedLaunchVelocityCustom;

	MovementFlagCustomMoveData = CurrentSavedMove.SavedMovementFlagCustom;
}

//Combines Flags together as an optimization option by the engine to send less data over the network
bool FCustomSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	FCustomSavedMove* NewMovePtr = static_cast<FCustomSavedMove*>(NewMove.Get());

	if (bWantsToSprint_Saved != NewMovePtr->bWantsToSprint_Saved)
	{
		return false;
	}

	if (bPressedForwardInput_Saved != NewMovePtr->bPressedForwardInput_Saved)
	{
		return false;
	}
	/*if (bWallRunIsRightSaved != NewMovePtr->bWallRunIsRightSaved)
	{
		return false;
	}*/

	

	/*if (SavedLaunchVelocityCustom != NewMovePtr->SavedLaunchVelocityCustom)
	{
		return false;
	}*/

	if (SavedMovementFlagCustom != NewMovePtr->SavedMovementFlagCustom)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

//Saves Move before Using
void FCustomSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	//This is where you set the saved move in case a packet is dropped containing this to minimize corrections
	UDensCharacterMovementComponent* CharacterMovement = Cast<UDensCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		bWantsToSprint_Saved = CharacterMovement->bWantsToSprint;
		bPressedForwardInput_Saved = CharacterMovement->bPressedForwardInput;
		//bWallRunIsRightSaved = CharacterMovement->bWallRunIsRight;

		//SavedLaunchVelocityCustom = CharacterMovement->LaunchVelocityCustom;

		//SavedMovementFlagCustom = CharacterMovement->MovementFlagCustom;

	}

}

//This is called usually when a packet is dropped and resets the compressed flag to its saved state
void FCustomSavedMove::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UDensCharacterMovementComponent* CharacterMovementComponent = Cast<UDensCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->bWantsToSprint = bWantsToSprint_Saved;
		
		CharacterMovementComponent->bPressedForwardInput = bPressedForwardInput_Saved;
		//CharacterMovementComponent->bWallRunIsRight = bWallRunIsRightSaved;

		//CharacterMovementComponent->LaunchVelocityCustom = SavedLaunchVelocityCustom;

		//CharacterMovementComponent->MovementFlagCustom = SavedMovementFlagCustom;

	}
}

//Just used to reset the data in a saved move.
void FCustomSavedMove::Clear()
{
	Super::Clear();

	bWantsToSprint_Saved = false;
	bPressedForwardInput_Saved = false;
	//bWallRunIsRightSaved = false;

	SavedLaunchVelocityCustom = FVector(0.f, 0.f, 0.f);


	SavedMovementFlagCustom = 0;
}



///// BOILERPLATE /////

//Acquires prediction data from clients (boilerplate code)
FNetworkPredictionData_Client* UDensCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UDensCharacterMovementComponent* MutableThis = const_cast<UDensCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FCustomNetworkPredictionData_Client(*this);
	}

	return ClientPredictionData;
}

//An older function used more in versions of Unreal prior to the introduction of Packed Movement Data (FCharacterNetworkMoveData).
//Can still be used for unpacking additional compressed flags within CustomSavedMove.
uint8 FCustomSavedMove::GetCompressedFlags() const
{

	return Super::GetCompressedFlags();

}

//Default constructor for FCustomNetworkPredictionData_Client. It's usually not necessary to populate this function.
FCustomNetworkPredictionData_Client::FCustomNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

//Generates a new saved move that will be populated and used by the system.
FSavedMovePtr FCustomNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FCustomSavedMove());
}

//The Flags parameter contains the compressed input flags that are stored in the parent saved move.
//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
//It basically just resets the movement component to the state when the move was made so it can simulate from there.
//We use ClientFillNetworkMoveData instead of this function, due to the limitation of the original flags system.
void UDensCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

/*
* Another boilerplate function that simply allows you to specify your custom move data class.
*/
FCustomCharacterNetworkMoveDataContainer::FCustomCharacterNetworkMoveDataContainer()
{
	NewMoveData = &CustomDefaultMoveData[0];
	PendingMoveData = &CustomDefaultMoveData[1];
	OldMoveData = &CustomDefaultMoveData[2];
}

#pragma endregion
/////END Networking/////