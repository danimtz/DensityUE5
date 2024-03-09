// Copyright 2023 Daniel Martinez Amigo. All Rights Reserved.


#include "DensCharacter.h"

#include "AbilitySystemGlobals.h"
#include "Ability/DensAbilitySystemComponent.h"
#include "Ability/DensAbilitySet.h"

#include "Ability/AttributeSets/DensCoreStatsAttributeSet.h"
#include "Ability/AttributeSets/DensBaseStatsAttributeSet.h"
#include "Ability/AttributeSets/DensAbilityEnergyAttributeSet.h"

#include "Player/DensPlayerState.h"
 	

#include "Kismet/GameplayStatics.h"

#include "Animation/AnimBlueprint.h"

#include "Weapons/DensWeapon.h"

#include "Characters/PlayerCharacters/DensCharacterMovementComponent.h"
#include "Characters/PlayerCharacters/DensPlayerCharacterData.h"
#include "Characters/PlayerCharacters/DensWeaponEquipmentComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInput/Public/InputAction.h"
#include "Input/DensInputComponent.h"
#include "DensGameplayTags.h"
#include "DensCombatComponent.h"
#include "Ability/DensAbilityTypes.h"
#include "Inventory/DensEquipmentManagerComponent.h"
#include "Inventory/DensInventoryManagerComponent.h"
#include "Player/DensPlayerController.h"
#include "ProfilingDebugging/StallDetector.h"
#include "UserInterface/DensHUDWidget.h"

// Sets default values
ADensCharacter::ADensCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
	DensCharacterMovementComponent = Cast<UDensCharacterMovementComponent>(GetCharacterMovement());
	if (DensCharacterMovementComponent)
	{
		DensCharacterMovementComponent->GravityScale = 1.5;
		DensCharacterMovementComponent->MaxAcceleration = 3048;
		DensCharacterMovementComponent->BrakingFrictionFactor = 0;
		DensCharacterMovementComponent->bUseSeparateBrakingFriction = true;
		DensCharacterMovementComponent->PerchRadiusThreshold = 30.0f;
		DensCharacterMovementComponent->bUseFlatBaseForFloorChecks = true;
		DensCharacterMovementComponent->JumpZVelocity = 750;
		DensCharacterMovementComponent->BrakingDecelerationFalling = 200;
		DensCharacterMovementComponent->AirControl = 0.25;
		DensCharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
		//DensCharacterMovementComponent->Hold
		
	}
	JumpMaxHoldTime = 0.25;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);


	//Setup subobjects

	RootSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("RootSpringArm"));
	if (RootSpringArm)
	{
		RootSpringArm->SetupAttachment(RootComponent);
		RootSpringArm->TargetArmLength = 0.0f;
		RootSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		RootSpringArm->bUsePawnControlRotation = true;
		RootSpringArm->bInheritPitch = false;
		RootSpringArm->bInheritYaw = true;
		RootSpringArm->bInheritRoll = false;
	}

	
	FirstPersonRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Root"));
	FirstPersonRoot->SetupAttachment(RootSpringArm);
	//FirstPersonRoot->SetIsReplicated(false);

	FPMeshRootSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Mesh_Root"));
	if (FPMeshRootSpringArm)
	{
		FPMeshRootSpringArm->SetupAttachment(FirstPersonRoot);
		FPMeshRootSpringArm->TargetArmLength = 0.0f;
		FPMeshRootSpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
		FPMeshRootSpringArm->bUsePawnControlRotation = true;
		FPMeshRootSpringArm->bInheritPitch = true;
		FPMeshRootSpringArm->bInheritYaw = true;
		FPMeshRootSpringArm->bInheritRoll = false;
	}
	
	/*
	OffsetRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Offset_Root"));
	if (OffsetRootComponent)
	{
		OffsetRootComponent->SetupAttachment(FPMeshRootSpringArm);
		OffsetRootComponent->SetRelativeLocation(FVector(3.0f, 0.0f, 0.0f));
	}
	*/
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	if(FirstPersonCamera)
	{
		FirstPersonCamera->SetupAttachment(FPMeshRootSpringArm);
		FirstPersonCamera->SetFieldOfView(77.0f);
		
	}

	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_FP"));
	if (FirstPersonMesh)
	{
		FirstPersonMesh->SetupAttachment(FirstPersonCamera);
		FirstPersonMesh->SetRelativeLocation(FVector(-3.0f, -17.0f, -149.0f));
		FirstPersonMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		FirstPersonMesh->CastShadow = false;
		FirstPersonMesh->SetOnlyOwnerSee(true);

		CharacterMeshes.Add(FirstPersonMesh);
	}
	
	FP_UpperArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperArms_FP"));
	if (FP_UpperArmsMesh)
	{
		FP_UpperArmsMesh->SetupAttachment(FirstPersonMesh);
		FP_UpperArmsMesh->SetLeaderPoseComponent(FirstPersonMesh);
		
		FP_UpperArmsMesh->CastShadow = false;
		FP_UpperArmsMesh->SetOnlyOwnerSee(true);

		CharacterMeshes.Add(FP_UpperArmsMesh);
	}


	GetMesh()->SetupAttachment(RootSpringArm);
	

	TP_ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms_ThirdPerson"));
	if (TP_ArmsMesh)
	{
		TP_ArmsMesh->SetupAttachment(GetMesh());
		TP_ArmsMesh->SetLeaderPoseComponent(GetMesh());
		//TP_ArmsMesh->SetOwnerNoSee(true);

		CharacterMeshes.Add(TP_ArmsMesh);
	}

	TP_ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest_ThirdPerson"));
	if (TP_ChestMesh)
	{
		TP_ChestMesh->SetupAttachment(GetMesh());
		TP_ChestMesh->SetLeaderPoseComponent(GetMesh());
		//TP_ChestMesh->SetOwnerNoSee(true);

		CharacterMeshes.Add(TP_ChestMesh);
	}

	TP_HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head_ThirdPerson"));
	if (TP_HeadMesh)
	{
		TP_HeadMesh->SetupAttachment(GetMesh());
		TP_HeadMesh->SetLeaderPoseComponent(GetMesh());
		//TP_HeadMesh->SetOwnerNoSee(true);

		CharacterMeshes.Add(TP_HeadMesh);
	}


	TP_ClassItem = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClassItem_ThirdPerson"));
	if (TP_ClassItem)
	{
		TP_ClassItem->SetupAttachment(GetMesh());
		TP_ClassItem->SetLeaderPoseComponent(GetMesh());
		//TP_HeadMesh->SetOwnerNoSee(true);

		CharacterMeshes.Add(TP_ClassItem);
	}

	
	

	WeaponEquipmentComponent = CreateDefaultSubobject<UDensWeaponEquipmentComponent>(TEXT("Equipped Weapons Component"));
	WeaponEquipmentComponent->SetIsReplicated(true);


	
	//OnCharacterDied.AddUniqueDynamic(WeaponEquipmentComponent, &UDensWeaponEquipmentComponent::OnPlayerDeath);
	//OnFirstThirdPersonSwapDelegate.AddUniqueDynamic(WeaponEquipmentComponent, &UDensWeaponEquipmentComponent::OnFirstToThirdPersonMeshSwap);
	//OnAbilitySystemInitializedDelegate.Add(FSimpleMulticastDelegate::FDelegate::CreateUObject(WeaponEquipmentComponent, &UDensWeaponEquipmentComponent::OnAbilitySystemInitialized));

	
	OnCharacterDied.AddUniqueDynamic(this, &ThisClass::OnPlayerDeath);
	
	OnAbilitySystemInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemInitialized);
	OnAbilitySystemUnInitializedDelegate.AddUObject(this, &ThisClass::OnAbilitySystemUnInitialized);
	
	//FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	
	OnFirstThirdPersonSwapDelegate.AddUniqueDynamic(this, &ThisClass::FirstToThirdPersonMeshVisibilitySwap);
	//OnFirstThirdPersonSwapDelegate.AddUniqueDynamic(WeaponEquipmentComponent, &UDensWeaponEquipmentComponent::OnFirstToThirdPersonMeshSwap);


	Faction = FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Faction.Guardian"));

	bIsInFirstPerson = false;
}

/*
UDensAbilitySystemComponent* ADensCharacter::GetDensAbilitySystemComponent() const
{
	return Cast<UDensAbilitySystemComponent>(GetAbilitySystemComponent());
}
*/


void ADensCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);

}


// Called when the game starts or when spawned
void ADensCharacter::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("Mvt Comp Class = %s"), *GetCharacterMovement()->GetClass()->GetName())


	//ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();

	//PS->GetKineticWeaponAttributeSet()->SetMaxMagSize(200.0f);

	//PS->GetEnergyWeaponAttributeSet()->SetMaxMagSize(150.0f);


	//Setup First person mesh attachment to camera transform component for camera shakes
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetController());
	if (PC)
	{
		
	}


	
}


void ADensCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);

}

void ADensCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WeaponEquipmentComponent->UpdateWeaponInstances(DeltaTime);
	
}


/*
UAbilitySystemComponent* ADensCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
*/




//Server init of ASC
void ADensCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
	if (PS)
	{
		//Set the ASC on the server. Clients in OnRep_PlayerState() function
		AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);


		//TODO: add more initalisation things of ASC... attributes?

		CoreStatsAttributeSet = PS->GetCoreStatsAttributeSet();
		BaseStatsAttributeSet = PS->GetBaseStatsAttributeSet();
		AbilityEnergyAttributeSet = PS->GetAbilityEnergyAttributeSet();
		
		if (BaseStatsAttributeSet.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Is valid"));
		}

		

		OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent.Get());

		ADensPlayerController* PC = Cast<ADensPlayerController>(NewController);
		if (PC)
		{
			FirstPersonMesh->AttachToComponent(PC->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
		
		/* MOVED TO PLAYER CONTROLLER ON POSSESSED
		PS->GetEquipmentManagerComponent()->UpdatePlayerEquipment();

		
		ADensPlayerController* PC = Cast<ADensPlayerController>(GetController());
		if (PC && IsLocallyControlled())
		{
			PC->InitHUD();
			PC->GetMainHUD()->FadeOutHealthBar(); //TODO add this to future OnPlayerSpawn delegate or something the sort
			
		}
		
		if (PC)
		{
			PC->GetInventoryManagerComponent()->InitInventory();
		}
		
		*/

		
	}

}



//Client init of ASC
void ADensCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
	if (PS)
	{
		
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if(ASC == nullptr)
		{
			// Set the ASC for clients. Server in PossessedBy.
			AbilitySystemComponent = Cast<UDensAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			
			// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);


			CoreStatsAttributeSet = PS->GetCoreStatsAttributeSet();
			BaseStatsAttributeSet = PS->GetBaseStatsAttributeSet();
			AbilityEnergyAttributeSet = PS->GetAbilityEnergyAttributeSet();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
		

		

		OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent.Get());
		
		

		
		ADensPlayerController* PC = Cast<ADensPlayerController>(GetController());
		if (PC && IsLocallyControlled() && PC->GetCharacter())
		{
			PC->InitHUD();
			PC->GetMainHUD()->FadeOutHealthBar(); //TODO add this to future OnPlayerSpawn delegate or something the sort
		}

		PS->GetEquipmentManagerComponent()->UpdatePlayerEquipment();
		PS->GetEquipmentManagerComponent()->RefreshPlayerEquipment();

		
		//Only Broadcast If PC is valid
		//if(PC)
		//{
		//OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent.Get());
		//}
		
		
		if (PC)
		{
			//TODO change how this is initted ITS BEING INNITED TWICE, ONCE IN ONREP CONTROLLER AND ANOTHER IN ONREP PLAYER STATE TO AVOID DATA RACE, BUT SHOULD ONLY BE RAN ONCE
			FirstPersonMesh->AttachToComponent(PC->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			if(PC->GetCharacter()) //If character hasnt been possessed yet dont init inventory now
			{
				PC->GetInventoryManagerComponent()->InitInventory();
			}
		}
		
		
		
	}

}

//Prevents Playercontroller data race if player controller wasnt valid when OnRep_Player state was called
void ADensCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Needed in case the PC wasn't valid when we Init-ed the ASC.
	ADensPlayerState* PS = GetPlayerState<ADensPlayerState>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->RefreshAbilityActorInfo();


		
		ADensPlayerController* PC = Cast<ADensPlayerController>(GetController());
		if (PC && IsLocallyControlled() && PC->GetCharacter())
		{
			PC->InitHUD();
			PC->GetMainHUD()->FadeOutHealthBar(); //TODO add this to future OnPlayerSpawn delegate or something the sort
		}
		
		PS->GetEquipmentManagerComponent()->UpdatePlayerEquipment();
		PS->GetEquipmentManagerComponent()->RefreshPlayerEquipment();
		

		//OnAbilitySystemInitializedDelegate.Broadcast(AbilitySystemComponent.Get());
		
		
		if (PC)
		{
			//TODO change how this is initted ITS BEING INNITED TWICE, ONCE IN ONREP CONTROLLER AND ANOTHER IN ONREP PLAYER STATE TO AVOID DATA RACE, BUT SHOULD ONLY BE RAN ONCE
			FirstPersonMesh->AttachToComponent(PC->PlayerCameraManager->GetTransformComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			PC->GetInventoryManagerComponent()->InitInventory();
		}
	}
}

void ADensCharacter::OnAbilitySystemInitialized(UDensAbilitySystemComponent* InASC)
{
	
	CombatComponent->OnOutOfHealth.AddUniqueDynamic(this, &ThisClass::HandleOutOfHealth);
	CombatComponent->OnDamageTaken.AddUniqueDynamic(this, &ThisClass::HandleDamageTaken);
	CombatComponent->OnOutOfShield.AddUniqueDynamic(this, &ThisClass::HandleOutOfShield);

	InASC->RegisterGameplayTagEvent(DensGameplayTags::State_Dying, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DyingTagChanged);

	
	InitializePlayerDataAbilities();

}



void ADensCharacter::OnAbilitySystemUnInitialized(UDensAbilitySystemComponent* InASC)
{
	
	/*CombatComponent->OnOutOfHealth.AddUniqueDynamic(this, &ThisClass::HandleOutOfHealth);
	CombatComponent->OnDamageTaken.AddUniqueDynamic(this, &ThisClass::HandleDamageTaken);
	CombatComponent->OnOutOfShield.AddUniqueDynamic(this, &ThisClass::HandleOutOfShield);

	InASC->RegisterGameplayTagEvent(DensGameplayTags::State_Dying, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DyingTagChanged);

	
	InitializePlayerDataAbilities();
	*/

	RemovePlayerDataAbilities();
}






// Called to bind functionality to input
void ADensCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); //Empty
	
	UDensInputComponent* DensIC = Cast<UDensInputComponent>(PlayerInputComponent);

	check(InputComponent);

	//const FDensGameplayTags& GameplayTags = FDensGameplayTags::Get();


	const UDensInputConfig* InputConfig = PlayerCharacterData->InputConfig;



	//TODO Add input Mappings
	TArray<uint32> BindHandles;
	DensIC->BindAbilityActions(InputConfig, this, &ADensCharacter::Input_AbilityInputTagPressed, &ADensCharacter::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	

	//Bind Input actions by tag
	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ADensCharacter::Input_Move, true);
	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Move, ETriggerEvent::Completed, this, &ADensCharacter::Input_MoveCompleted, true);

	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ADensCharacter::Input_Look_Mouse, true);

	//Jumps
	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Jump, ETriggerEvent::Started, this, &ADensCharacter::Input_JumpStart, true);
	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ADensCharacter::Input_JumpTriggered, true);
	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Jump, ETriggerEvent::Completed, this, &ADensCharacter::Input_JumpCompleted, true);

	DensIC->BindNativeActionByTag(InputConfig, DensGameplayTags::InputTag_Crouch, ETriggerEvent::Started, this, &ADensCharacter::Input_Crouch, true);

	//DensIC->BindNativeActionByTag(InputConfig, GameplayTags.InputTag_Sprint, ETriggerEvent::Started, this, &ADensCharacter::Input_Sprint, true);



	


}




void ADensCharacter::InitializePlayerDataAbilities()
{

	//Bind Abilities. NOT SURE IF THIS SHOULD GO HERE //@TODO: AbilitySet should be part of player state perhaps.
	if (AbilitySystemComponent.IsValid())
	{
		
		for (auto &AbilitySet : PlayerCharacterData->AbilitySets)
		{
			if (AbilitySet)
			{
				UDensAbilitySystemComponent* DensASC = AbilitySystemComponent.Get();
				AbilitySet->AddToASC(DensASC, &GrantedAbilityHandles);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AbilitySet not found or not set when adding abilities for DensCharacter  [%s] ."), *GetNameSafe(this));
			}
		}

	}

}


void ADensCharacter::RemovePlayerDataAbilities()
{
	if (AbilitySystemComponent.IsValid() && HasAuthority())
	{
		
		/*for (auto &AbilitySet : PlayerCharacterData->AbilitySets)
		{
			if (AbilitySet)
			{
				UDensAbilitySystemComponent* DensASC = AbilitySystemComponent.Get();
				AbilitySet->RemoveFromASC(DensASC);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AbilitySet not found or not set when removing abilities for DensCharacter  [%s] ."), *GetNameSafe(this));
			}
		}
		*/
		GrantedAbilityHandles.RemoveAbilitiesFromASC(GetDensAbilitySystemComponent());
	}
}


//Overrides the base OnStartCrouch
void ADensCharacter::OnStartCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	
	Super::OnStartCrouch(HeightAdjust, ScaledHeightAdjust);
	// Apply base translation offset to FPRoot as well
	
	const ADensCharacter* DefaultChar = GetDefault<ADensCharacter>(GetClass());
	if (RootSpringArm && DefaultChar->RootSpringArm)
	{

		FVector& RootRelativeLocation = RootSpringArm->GetRelativeLocation_DirectMutable();
		RootRelativeLocation.Z = DefaultChar->RootSpringArm->GetRelativeLocation().Z + HeightAdjust;

		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;

		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	
	
	
	SmoothCrouch();//BP event

}


//Overrides the base OnEndCrouch
void ADensCharacter::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	
	Super::OnEndCrouch(HeightAdjust, ScaledHeightAdjust);
	
	// Apply base translation offset to FPRoot as well
	
	//RecalculateBaseEyeHeight();
	
	const ADensCharacter* DefaultChar = GetDefault<ADensCharacter>(GetClass());
	if (RootSpringArm && DefaultChar->RootSpringArm)
	{
		FVector& RootRelativeLocation = RootSpringArm->GetRelativeLocation_DirectMutable();
		RootRelativeLocation.Z = DefaultChar->RootSpringArm->GetRelativeLocation().Z;

		
	}

	
	SmoothCrouch();//BP event

}



bool ADensCharacter::CanJumpInternal_Implementation() const
{
	return JumpIsAllowedInternal();
}

void ADensCharacter::HandleOutOfShield(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	Super::HandleOutOfShield(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude);

	if (AbilitySystemComponent.Get())
	{
		{
			FGameplayCueParameters CueParameters;
			CueParameters.OriginalTag = DensGameplayTags::GameplayCue_Character_ShieldBroken;
			FDensGameplayEffectContext* EffectContext = static_cast<FDensGameplayEffectContext*>(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
			
			FGameplayEffectContextHandle ContextHandle = FGameplayEffectContextHandle(EffectContext);
			CueParameters.EffectContext = ContextHandle;
			CueParameters.Instigator = DamageInstigator;
			CueParameters.AggregatedSourceTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			CueParameters.AggregatedTargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			AbilitySystemComponent->ExecuteGameplayCue(DensGameplayTags::GameplayCue_Character_ShieldBroken, DamageEffectSpec.GetEffectContext());
		}
	}
}


//====== FirstToThird Person Swap ========

void ADensCharacter::PerspectiveSwap(bool bIsFirstPerson)
{
	bIsInFirstPerson = bIsFirstPerson;
	
	OnFirstThirdPersonSwapDelegate.Broadcast(bIsFirstPerson);
}

UDensWeaponEquipmentComponent* ADensCharacter::GetWeaponEquipmentComponent() const
{
	if(WeaponEquipmentComponent)
	{
		return WeaponEquipmentComponent;
	}
	return nullptr;
}


void ADensCharacter::FirstToThirdPersonMeshVisibilitySwap(bool bIsFirstPerson)
{
	if(IsLocallyControlled()){
		FirstPersonMesh->SetOwnerNoSee(!bIsFirstPerson);
		FP_UpperArmsMesh->SetOwnerNoSee(!bIsFirstPerson);
		
		//GetMesh()->SetOwnerNoSee(bIsFirstPerson);
		TP_ArmsMesh->SetOwnerNoSee(bIsFirstPerson);
		TP_ChestMesh->SetOwnerNoSee(bIsFirstPerson);
		TP_HeadMesh->SetOwnerNoSee(bIsFirstPerson);
		TP_ClassItem->SetOwnerNoSee(bIsFirstPerson);
		
		RootSpringArm->bEnableCameraLag = bIsFirstPerson;
	}
	
}




void ADensCharacter::OnPlayerDeath(ADensCharacterBase* Character, FCharacterDeathInfo DeathInfo)
{
	ADensPlayerController* PC = Cast<ADensPlayerController>(GetController());
	if(PC)
	{
		PC->OnPlayerDeath_CLIENT(DeathInfo);
		PC->OnPlayerDeath();
	}
	
}




FVector ADensCharacter::GetTargetingLocation()
{

	FVector TargetingLocation = FPMeshRootSpringArm->GetComponentTransform().GetLocation();
	return TargetingLocation;
}


//====== Armor meshes setters ========

void ADensCharacter::SetHeadMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		TP_HeadMesh->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetArmsMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		TP_ArmsMesh->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetArmsFPMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		FirstPersonMesh->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetChestMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		TP_ChestMesh->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetChestFPMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		FP_UpperArmsMesh->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetLegsMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	if(NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

void ADensCharacter::SetClassItemMesh(TObjectPtr<USkeletalMesh> NewMesh)
{
	
	if(NewMesh)
	{
		TP_ClassItem->SetSkeletalMesh(NewMesh);
	}
}




//====== Input Functions (could be moved to a component) ========



void ADensCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}



void ADensCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}


void ADensCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr) 
	{
		const FVector2D MoveValue = InputActionValue.Get<FVector2D>();

		

		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (MoveValue.X != 0.0f) 
		{
			

			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			float HorizontalModifier = DensCharacterMovementComponent->bWantsToSprint ? HorizontalSprintSpeedModifier : HorizontalSpeedModifier;

			float MappedValue = FMath::GetMappedRangeValueClamped(FVector2D(1.0, -1.0), FVector2D(HorizontalModifier, -HorizontalModifier), MoveValue.X);

			//UE_LOG(LogTemp, Warning, TEXT("Move X, %f"), MappedValue);

			AddMovementInput(MovementDirection, MappedValue);
			
			
		}
		
		if (MoveValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, MoveValue.Y);
			DensCharacterMovementComponent->ForwardInputPressed();

			//If Player moves backwards stop sprint
			if (MoveValue.Y < 0.0f)
			{
				DensCharacterMovementComponent->CancelSprintAbility();
				
				DensCharacterMovementComponent->ForwardInputReleased(); //Used for mantle
			}
		}
		else //If forward/backwards not pressed stop sprint
		{
			DensCharacterMovementComponent->CancelSprintAbility();

			DensCharacterMovementComponent->ForwardInputReleased(); //Used for mantle
		}

	}
}

void ADensCharacter::Input_MoveCompleted(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr)
	{
		DensCharacterMovementComponent->CancelSprintAbility();
		DensCharacterMovementComponent->ForwardInputReleased();
	}
}

void ADensCharacter::Input_Look_Mouse(const FInputActionValue& InputActionValue)
{
	if (Controller != nullptr) 
	{
		
		const FVector2D LookValue = InputActionValue.Get<FVector2D>();

		AddControllerYawInput(LookValue.X);

		AddControllerPitchInput(LookValue.Y);

	}
}

void ADensCharacter::Input_JumpStart(const FInputActionValue& InputActionValue)
{
	DensCharacterMovementComponent->StopCrouch(); //make sure this doenst stop sprint
}

void ADensCharacter::Input_JumpTriggered(const FInputActionValue& InputActionValue)
{
	
	Jump();

	if (DensCharacterMovementComponent->IsMovingOnGround())
	{
		OnGroundedJumpTriggered();
	}
}

void ADensCharacter::Input_JumpCompleted(const FInputActionValue& InputActionValue)
{
	StopJumping();
}

void ADensCharacter::Input_Crouch(const FInputActionValue& InputActionValue)
{
	
	DensCharacterMovementComponent->ToggleCrouch();
	
}


/*void ADensCharacter::Input_Sprint(const FInputActionValue& InputActionValue)
{
	DensCharacterMovementComponent->StopCrouch();
	//DensCharacterMovementComponent->ToggleSprint();
}*/




