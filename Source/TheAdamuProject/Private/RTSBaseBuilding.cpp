// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseBuilding.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ARTSBaseBuilding::ARTSBaseBuilding()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

