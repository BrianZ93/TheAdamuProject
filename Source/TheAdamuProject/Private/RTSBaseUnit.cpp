// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseUnit.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"

// Sets default values
ARTSBaseUnit::ARTSBaseUnit()
{
	// Ensure actor can replicate
	SetReplicates(true);
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsMoving = false;
	MovementTarget = FVector::ZeroVector;

	// Initialize new properties
	bIsMining = false;
	CurrentGoldResource = nullptr;
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// Disable auto-possession by any Player Controller
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	// Create and attach the Enemy decal component
	SelectionDecal_Enemy = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_Enemy"));
	SelectionDecal_Enemy->SetupAttachment(RootComponent);
	SelectionDecal_Enemy->SetVisibility(false); // Hidden by default

	// Create and attach the PlayerOwned decal component
	SelectionDecal_PlayerOwned = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_PlayerOwned"));
	SelectionDecal_PlayerOwned->SetupAttachment(RootComponent);
	SelectionDecal_PlayerOwned->SetVisibility(false); // Hidden by default

	// Create and attach the Prospect decal component
	SelectionDecal_Prospect = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_Prospect"));
	SelectionDecal_Prospect->SetupAttachment(RootComponent);
	SelectionDecal_Prospect->SetVisibility(false); // Hidden by default

	// Configure CharacterMovementComponent (default setup for ground units)
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate toward movement direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // Fast rotation
	GetCharacterMovement()->MaxWalkSpeed = CustomMoveSpeed; // Use custom speed
	GetCharacterMovement()->bUseRVOAvoidance = true; // Enable basic unit avoidance
}

void ARTSBaseUnit::BeginPlay()
{
	Super::BeginPlay();

	// Apply the Animation Blueprint to the mesh if specified
	if (UnitAnimationBlueprint)
	{
		GetMesh()->SetAnimInstanceClass(UnitAnimationBlueprint);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Animation Blueprint assigned to %s"), *GetName());
	}
}


void ARTSBaseUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSBaseUnit, OwningPlayerId);
	DOREPLIFETIME(ARTSBaseUnit, TeamNumber);
}

void ARTSBaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		UpdateMovement(DeltaTime);
	}
}

// Function to toggle decals based on selection state
void ARTSBaseUnit::SetSelectionState(bool bIsSelected, bool bIsPlayerOwned, bool bIsProspect)
{
	if (bIsSelected)
	{
		if (bIsPlayerOwned)
		{
			SelectionDecal_PlayerOwned->SetVisibility(true);
			SelectionDecal_Enemy->SetVisibility(false);
			SelectionDecal_Prospect->SetVisibility(false);
			UE_LOG(LogTemp, Log, TEXT("Selecting Player Owned Unit Unit"));
		}
		else if (bIsProspect)
		{
			SelectionDecal_Prospect->SetVisibility(true);
			SelectionDecal_Enemy->SetVisibility(false);
			SelectionDecal_PlayerOwned->SetVisibility(false);
			UE_LOG(LogTemp, Log, TEXT("Selecting prospective Unit Unit"));
		}
		else // Enemy by default
		{
			SelectionDecal_Enemy->SetVisibility(true);
			SelectionDecal_PlayerOwned->SetVisibility(false);
			SelectionDecal_Prospect->SetVisibility(false);
			UE_LOG(LogTemp, Log, TEXT("Selecting Enemy Owned Unit Unit"));
		}
	}
	else
	{
		// Hide all decals when not selected
		SelectionDecal_Enemy->SetVisibility(false);
		SelectionDecal_PlayerOwned->SetVisibility(false);
		SelectionDecal_Prospect->SetVisibility(false);
	}
}

void ARTSBaseUnit::SetMovementTarget(FVector NewTarget, AActor* TargetActor, EResourceType ResourceType) 
{
	// Empty base implementation, to be overriden
}

void ARTSBaseUnit::UpdateMovement(float DeltaTime)
{
	// Empty base implementation, to be overriden
}

void ARTSBaseUnit::StartGoldMining(AActor* GoldResource)
{
	CurrentGoldResource = GoldResource;
	bIsMining = true;
	UE_LOG(LogTemp, Log, TEXT("Unit %s started mining resource %s"), *GetName(), *GoldResource->GetName());
}
