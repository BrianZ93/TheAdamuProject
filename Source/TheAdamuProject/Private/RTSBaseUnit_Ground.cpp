// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseUnit_Ground.h"
#include "RTSBaseBuilding.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RTSResourceInteractionInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "RTS_AIController.h"
#include "Components/CapsuleComponent.h"

ARTSBaseUnit_Ground::ARTSBaseUnit_Ground()
{
    AIControllerClass = ARTS_AIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Ground-specific initialization
    OwningPlayerId = 0;
    GetCharacterMovement()->MaxWalkSpeed = CustomMoveSpeed;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    bIsPathfinding = false;
    bIsMoving = false;
    bIsMining = false;
}

void ARTSBaseUnit_Ground::BeginPlay()
{
    Super::BeginPlay();

    // Manually possess if no controller is found
    if (!GetController())
    {
        ARTS_AIController* NewController = GetWorld()->SpawnActor<ARTS_AIController>(ARTS_AIController::StaticClass());
        if (NewController)
        {
            NewController->Possess(this);
            AIController = NewController;
            UE_LOG(LogTemp, Log, TEXT("Manually possessed unit %s with RTS_AIController"), *GetName());
        }
    }
    else
    {
        AIController = Cast<ARTS_AIController>(GetController());
        if (AIController)
        {
            UE_LOG(LogTemp, Log, TEXT("RTS_AIController successfully attached to unit %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("RTS_AIController not found for unit %s. Controller is: %s"),
                *GetName(), GetController() ? *GetController()->GetClass()->GetName() : TEXT("Null"));
        }
    }

    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementComponent)
    {
        MovementComponent->bUseRVOAvoidance = true;
        MovementComponent->AvoidanceConsiderationRadius = 200.0f; // Increased for maze navigation
        MovementComponent->AvoidanceWeight = 0.8f; // Stronger avoidance
        MovementComponent->SetAvoidanceGroup(1);
        MovementComponent->SetGroupsToAvoid(1);
        MovementComponent->SetGroupsToIgnore(0);
        MovementComponent->bOrientRotationToMovement = true; // Align with movement direction
        MovementComponent->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Fast rotation for responsiveness
    }

}

void ARTSBaseUnit_Ground::SetMovementTarget(FVector NewTarget, AActor* TargetActor, EResourceType ResourceType)
{
    MovementTarget = NewTarget;
    bIsMoving = true;
    bIsMining = false;
    bIsPathfinding = true;

    if (ResourceType == EResourceType::Gold)
    {
        CurrentGoldResource = TargetActor;
        CurrentResourceType = ResourceType;
    }
    else
    {
        CurrentGoldResource = nullptr;
		CurrentResourceType = EResourceType::None;
    }

    if (AIController)
    {
        AIController->MoveToTargetLocation(MovementTarget, 50.0f);
        UE_LOG(LogTemp, Log, TEXT("Unit %s set movement target to %s"), *GetName(), *MovementTarget.ToString());

        // Debug: Draw the target location
        DrawDebugSphere(GetWorld(), MovementTarget, 50.0f, 12, FColor::Red, false, 2.0f);
    }
}

void ARTSBaseUnit_Ground::UpdateMovement(float DeltaTime)
{
    if (!AIController || !bIsMoving)
    {
        return;
    }

    // Periodically recalculate path to handle dynamic obstacles
    static float PathRecalcTimer = 0.f;
    PathRecalcTimer += DeltaTime;
    if (PathRecalcTimer >= 1.0f) // Recalculate every second
    {
        PathRecalcTimer = 0.f;
        AIController->MoveToTargetLocation(MovementTarget, 50.f);
        UE_LOG(LogTemp, Log, TEXT("Unit %s recalculating path to target"), *GetName());
    }

    float DistanceToTarget = FVector::Dist(GetActorLocation(), MovementTarget);
    if (DistanceToTarget <= 150.f) // Acceptance radius
    {
        AIController->StopUnitMovement();
        bIsMoving = false; // Ensure movement stops

        // Check if targeting a gold resource
        if (CurrentGoldResource && CurrentGoldResource->Implements<URTSResourceInteractionInterface>() &&
            IRTSResourceInteractionInterface::Execute_IsGoldResource(CurrentGoldResource))
        {
            FVector Direction = (CurrentGoldResource->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(Direction);
            SetActorRotation(TargetRotation);
            StartGoldMining(CurrentGoldResource);
            bIsMining = true;
            UE_LOG(LogTemp, Log, TEXT("Unit %s reached gold resource and started mining %s"), *GetName(), *CurrentGoldResource->GetName());
        }
    }
}

void ARTSBaseUnit_Ground::StartGoldMining(AActor* GoldResource)
{
    // Call the base class implementation
    Super::StartGoldMining(GoldResource);
}

void ARTSBaseUnit_Ground::FindNearestGoldDepositLocation_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Unit %s stopped mining and is looking for a drop-off"), *GetName());

    // Step 1: Get the navigation system
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Navigation system not found for unit %s"), *GetName());
        return;
    }

    // Step 2: Find all actors in the world
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    UE_LOG(LogTemp, Log, TEXT("Total actors found in world: %d"), AllActors.Num());

    if (AllActors.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No actors found in the world for unit %s"), *GetName());
        return;
    }

    // Step 3: Filter actors and calculate shortest path
    AActor* NearestDropoff = nullptr;
    float ShortestPathLength = MAX_FLT;
    FVector UnitLocation = GetActorLocation();

    for (AActor* Actor : AllActors)
    {
        if (!Actor || Actor == this)
        {
            continue;
        }

        // Log all actors for debugging
        UE_LOG(LogTemp, Log, TEXT("Checking actor: %s"), *Actor->GetName());

        // Check if the actor implements IRTSResourceInteractionInterface
        if (!Actor->Implements<URTSResourceInteractionInterface>())
        {
            UE_LOG(LogTemp, Log, TEXT("Actor %s does not implement IRTSResourceInteractionInterface"), *Actor->GetName());
            continue;
        }

        // Check if the actor can receive gold
        bool bCanReceiveGold = IRTSResourceInteractionInterface::Execute_BuildingCanReceiveGold(Actor);
        if (!bCanReceiveGold)
        {
            UE_LOG(LogTemp, Log, TEXT("Actor %s cannot receive gold"), *Actor->GetName());
            continue;
        }

        // Calculate the path length to this actor
        FPathFindingQuery Query;
        Query.StartLocation = UnitLocation;
        Query.EndLocation = Actor->GetActorLocation();
        Query.NavData = NavSystem->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
        Query.Owner = this;

        FPathFindingResult PathResult = NavSystem->FindPathSync(Query);
        if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
        {
            float PathLength = PathResult.Path->GetLength();
            UE_LOG(LogTemp, Log, TEXT("Valid path to %s, length: %f"), *Actor->GetName(), PathLength);
            if (PathLength < ShortestPathLength)
            {
                ShortestPathLength = PathLength;
                NearestDropoff = Actor;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No valid path found to %s from unit %s"), *Actor->GetName(), *GetName());
        }
    }

    // Step 4: Process the result
    if (NearestDropoff)
    {
        UE_LOG(LogTemp, Log, TEXT("Unit %s found nearest drop-off: %s (Path Length: %f)"),
            *GetName(), *NearestDropoff->GetName(), ShortestPathLength);

        DrawDebugSphere(
            GetWorld(),
            NearestDropoff->GetActorLocation(),
            100.0f,
            12,
            FColor::Green,
            false,
            5.0f
        );

        // Move to the drop-off
        SetMovementTarget(NearestDropoff->GetActorLocation(), NearestDropoff, EResourceType::None);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid drop-off location found for unit %s"), *GetName());
    }
}

void ARTSBaseUnit_Ground::StopMiningAndDropoffGold()
{
    // Stop mining
    bIsMining = false;
    CurrentGoldResource = nullptr;
    CurrentResourceType = EResourceType::None;

    UE_LOG(LogTemp, Log, TEXT("Unit %s stopped mining and is looking for a drop-off"), *GetName());

    // Trigger drop-off logic
    FindNearestGoldDepositLocation_Implementation();
}
