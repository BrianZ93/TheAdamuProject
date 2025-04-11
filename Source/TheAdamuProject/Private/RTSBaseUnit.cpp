// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseUnit.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ARTSBaseUnit::ARTSBaseUnit()
{
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = true;
	bIsMoving = false;
	MovementTarget = FVector::ZeroVector;
	bIsMining = false;
	CurrentGoldResource = nullptr;

	// Set CapsuleComponent as root
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleSize(35.f, 90.f); // Adjust size as needed

	// Attach the SkeletalMesh to the BoxComponent
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Mesh is visual-only
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// Configure CharacterMovementComponent (default setup for ground units)
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = CustomMoveSpeed;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

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
