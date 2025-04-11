// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseBuilding.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ARTSBaseBuilding::ARTSBaseBuilding()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the BoxComponent and set it as the root
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	// Configure collision settings for the BoxComponent
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECC_WorldStatic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetBoxExtent(FVector(100.f, 100.f, 100.f)); // Default size, adjust in BP_TownHall_C
	CollisionBox->CanCharacterStepUpOn = ECB_No;

	// Create the StaticMeshComponent and attach it to the CollisionBox
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	BuildingMesh->SetupAttachment(CollisionBox);
	BuildingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Mesh is visual-only now

	// Create and attach the Enemy decal component
	SelectionDecal_Enemy = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_Enemy"));
	SelectionDecal_Enemy->SetupAttachment(CollisionBox); // Attach to root
	SelectionDecal_Enemy->SetVisibility(false);

	// Create and attach the PlayerOwned decal component
	SelectionDecal_PlayerOwned = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_PlayerOwned"));
	SelectionDecal_PlayerOwned->SetupAttachment(CollisionBox); // Attach to root
	SelectionDecal_PlayerOwned->SetVisibility(false);

	// Create and attach the Prospect decal component
	SelectionDecal_Prospect = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal_Prospect"));
	SelectionDecal_Prospect->SetupAttachment(CollisionBox); // Attach to root
	SelectionDecal_Prospect->SetVisibility(false);
}

// Called when the game starts or when spawned
void ARTSBaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARTSBaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARTSBaseBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSBaseBuilding, OwningPlayerId);
	DOREPLIFETIME(ARTSBaseBuilding, TeamNumber);
}

// Function to toggle decals based on selection state
void ARTSBaseBuilding::SetSelectionState(bool bIsSelected, bool bIsPlayerOwned, bool bIsProspect)
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

bool ARTSBaseBuilding::BuildingCanReceiveGold_Implementation()
{
	// Minimal fallback: log a warning and return false
	UE_LOG(LogTemp, Warning, TEXT("BuildingCanReceiveGold_Implementation called on %s - Blueprint override expected!"), *GetName());
	return false;
}

