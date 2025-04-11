// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSGameplayManager.h"
#include "RTSBaseBuilding.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARTSGameplayManager::ARTSGameplayManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARTSGameplayManager::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ARTSGameplayManager::SpawnActorAndAddToPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, APlayerController* PlayerController)
{
    if (!ActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActorAndAddToPool: ActorClass is null."));
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActorAndAddToPool: World is null."));
        return nullptr;
    }

    // Prepare spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = PlayerController ? PlayerController->GetPawn() : nullptr;
    SpawnParams.Instigator = PlayerController ? PlayerController->GetPawn() : nullptr;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Spawn the actor using the full transform
    AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);
    if (!SpawnedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActorAndAddToPool: Failed to spawn actor."));
        return nullptr;
    }

    // Check if the spawned actor is an RTSBaseBuilding
    if (SpawnedActor->IsA<ARTSBaseBuilding>())
    {
        // Define the extent for navmesh rebuilding (adjust as needed)
        FVector BoxExtent = FVector(500.0f, 500.0f, 100.0f); // Example: 5m x 5m x 1m box
        RebuildNavMeshAroundLocation(SpawnTransform.GetLocation(), BoxExtent);
    }

    return SpawnedActor;
}

void ARTSGameplayManager::RebuildNavMeshAroundLocation(const FVector& Location, const FVector& BoxExtent)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("RebuildNavMeshAroundLocation: World is null."));
        return;
    }

    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (NavSystem)
    {
        // Define the bounds for the dirty area
        FBox DirtyBounds(ForceInit);
        DirtyBounds += Location - BoxExtent;
        DirtyBounds += Location + BoxExtent;

        // Mark the area as dirty to trigger a navmesh rebuild
        NavSystem->AddDirtyArea(DirtyBounds, ENavigationDirtyFlag::All, FName(TEXT("BuildingSpawned")));
        UE_LOG(LogTemp, Log, TEXT("NavMesh marked dirty around location: %s"), *Location.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RebuildNavMeshAroundLocation: Navigation system not found."));
    }
}
