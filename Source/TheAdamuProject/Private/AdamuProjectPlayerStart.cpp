// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectPlayerStart.h"
#include "AdamuProjectGameModeBase.h"
#include "AdamuProjectPlayerState.h"
#include "AdamuProjectPlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

AAdamuProjectPlayerStart::AAdamuProjectPlayerStart(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default to Human race
    SelectedRace = ERaceType::Human;

    // Get the default capsule component from APlayerStart
    UCapsuleComponent* DefaultCapsule = Cast<UCapsuleComponent>(GetRootComponent());
    if (DefaultCapsule)
    {
        // Disable collision to prevent interference with spawning
        DefaultCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DefaultCapsule->SetGenerateOverlapEvents(false);
    }
}

void AAdamuProjectPlayerStart::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    UWorld* World = GetWorld();
    if (!World || World->IsGameWorld()) // Only run in editor, not in-game
    {
        return;
    }

    FVector SpawnLocation = GetActorLocation();
    FCollisionShape BuildingCollisionShape;

    // Check if the location is valid for spawning
    bool bIsValid = IsSpawnLocationValid(SpawnLocation, BuildingCollisionShape);

#if WITH_EDITOR
    // Provide visual feedback in the editor
    FColor DebugColor = bIsValid ? FColor::Green : FColor::Red;
    DrawDebugBox(World, SpawnLocation, BuildingCollisionShape.GetExtent(), DebugColor, false, -1.f, 0, 2.f);

    // log a message in the editor output log
    if (!bIsValid)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerStart at %s is not placeable - obstructed"), *SpawnLocation.ToString());
    }
#endif
}

void AAdamuProjectPlayerStart::MovePlayerCameraToStartLocation()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PlayerController = nullptr;
    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PC = Iterator->Get();
        if (PC && PC->PlayerState)
        {
            PlayerController = PC;
            break;
        }
    }

    if (PlayerController)
    {
        if (AAdamuProjectPlayerCameraManager* CameraManager = Cast<AAdamuProjectPlayerCameraManager>(PlayerController->PlayerCameraManager))
        {
            // Get the current camera location to preserve the Z height
            FVector CurrentCameraLocation = CameraManager->GetCameraLocation();

            // Get the PlayerStart's location and only use X and Y
            FVector PlayerStartLocation = GetActorLocation();
            FVector NewCameraLocation = FVector(
                PlayerStartLocation.X,  // Center X on PlayerStart
                PlayerStartLocation.Y,  // Center Y on PlayerStart
                CurrentCameraLocation.Z // Preserve current Z height
            );

            // Set the new camera location
            CameraManager->SetCameraLocation(NewCameraLocation);

            // Set rotation for a top-down view: pitch = -90 (look straight down), reset yaw to original
            CameraManager->SetCameraRotation(FRotator(-90.f, CameraManager->GetOriginalYaw(), 0.f));

            UE_LOG(LogTemp, Log, TEXT("Moved camera to PlayerStart X,Y location: %s with top-down rotation"), *NewCameraLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerCameraManager to AAdamuProjectPlayerCameraManager"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerController found to move camera"));
    }
}

bool AAdamuProjectPlayerStart::IsSpawnLocationValid(const FVector& Location, FCollisionShape& OutCollisionShape) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    // Use the selected race's building class to determine the collision size
    const FPlayerStartRaceConfig& RaceConfig = (SelectedRace == ERaceType::Human) ? HumanConfig : ApkalluConfig;
    if (!RaceConfig.BuildingClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("No BuildingClass defined for %s in PlayerStart"),
            (SelectedRace == ERaceType::Human) ? TEXT("Human") : TEXT("Apkallu"));
        OutCollisionShape = FCollisionShape::MakeBox(FVector(50.f, 50.f, 50.f)); // Fallback size
        return false;
    }

    // Get the building's collision bounds from its default object
    AActor* DefaultBuilding = RaceConfig.BuildingClass->GetDefaultObject<AActor>();
    if (!DefaultBuilding)
    {
        OutCollisionShape = FCollisionShape::MakeBox(FVector(150.f, 150.f, 150.f)); // Default size
        return false;
    }

    FVector BuildingOrigin;
    FVector BuildingExtent;
    DefaultBuilding->GetActorBounds(false, BuildingOrigin, BuildingExtent);
    OutCollisionShape = FCollisionShape::MakeBox(BuildingExtent * 1.1f); // Add 10% buffer

    // Perform a sweep test to check for obstructions
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(const_cast<AAdamuProjectPlayerStart*>(this)); // Ignore this actor

    FHitResult Hit;
    bool bIsObstructed = World->SweepSingleByChannel(
        Hit,
        Location,
        Location,
        FQuat::Identity,
        ECC_WorldStatic,
        OutCollisionShape,
        QueryParams
    );

    if (bIsObstructed)
    {
        UE_LOG(LogTemp, Log, TEXT("PlayerStart at %s is obstructed by %s"), *Location.ToString(), *Hit.GetActor()->GetName());
        return false;
    }

    return true;
}

void AAdamuProjectPlayerStart::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("RTSPlayerStart BeginPlay called"));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World is null"));
        return;
    }

    // Call Blueprint-implemented function to spawn building and workers
    SpawnMainBuildingAndWorkers();

    MovePlayerCameraToStartLocation();
}