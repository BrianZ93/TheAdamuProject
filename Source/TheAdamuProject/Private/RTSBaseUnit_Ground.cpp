// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseUnit_Ground.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RTSResourceInteractionInterface.h"
#include "Kismet/KismetMathLibrary.h"

ARTSBaseUnit_Ground::ARTSBaseUnit_Ground()
{
    // Create and attach an AIController
    OwningPlayerId = 0;

    // Ground-specific initialization
    GetCharacterMovement()->MaxWalkSpeed = CustomMoveSpeed; // Ensure speed is set
    GetCharacterMovement()->bUseControllerDesiredRotation = false; // We'll handle direction

    // Create an AIController for pathfinding
    AIController = nullptr;
    bIsPathfinding = false;
    bIsMoving = false;

    // Spawn an AIController and possess this character
    AIController = GetWorld() ? GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass()) : nullptr;
    if (AIController)
    {
        AIController->Possess(this);
    }
}

void ARTSBaseUnit_Ground::BeginPlay()
{
    Super::BeginPlay();
    if (!AIController)
    {
        AIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass());
        if (AIController)
        {
            AIController->Possess(this);
        }
    }
}

void ARTSBaseUnit_Ground::SetMovementTarget(FVector NewTarget, AActor* TargetActor, EResourceType ResourceType)
{
    MovementTarget = NewTarget;
    bIsMoving = true;
    bIsPathfinding = true;
    bIsMining = false;

    // Set resource based on type
    if (ResourceType == EResourceType::Gold)
    {
        CurrentGoldResource = TargetActor;
    }
    else
    {
        CurrentGoldResource = nullptr; // Clear if not gold (add other resource types later)
    }


    UE_LOG(LogTemp, Log, TEXT("Ground Unit %s moving to: %s (Target Actor: %s)"),
        *GetName(), *MovementTarget.ToString(), TargetActor ? *TargetActor->GetName() : TEXT("None"));

    // Use the navigation system to move to the target
    if (AIController)
    {
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavSystem)
        {
            AIController->MoveToLocation(MovementTarget, 50.0f);  // Acceptance radius
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Navigation system not found for unit %s"), *GetName());
            bIsMoving = false;
            bIsPathfinding = false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AIController not found for unit %s"), *GetName());
        bIsMoving = false;
        bIsPathfinding = false;
    }

}

void ARTSBaseUnit_Ground::UpdateMovement(float DeltaTime)
{
    if (!bIsPathfinding)
    {
        return; // Already handled by AIController
    }

    // Check if we've reached the target
    if (AIController)
    {
        EPathFollowingStatus::Type PathStatus = AIController->GetPathFollowingComponent()->GetStatus();
        if (PathStatus == EPathFollowingStatus::Idle)
        {
            // Reached target or failed to find a path
            bIsMoving = false;
            bIsPathfinding = false;
            GetCharacterMovement()->StopMovementImmediately();
            //SetActorLocation(FVector(MovementTarget.X, MovementTarget.Y, GetActorLocation().Z)); // Snap to target (keep Z)
            UE_LOG(LogTemp, Log, TEXT("Ground Unit %s reached target: %s"), *GetName(), *MovementTarget.ToString());

            // If targeting a gold resource, rotate and start mining
            if (CurrentGoldResource && CurrentGoldResource->Implements<URTSResourceInteractionInterface>() &&
                IRTSResourceInteractionInterface::Execute_IsGoldResource(CurrentGoldResource))
            {
                // Rotate toward the resource
                FVector Direction = (CurrentGoldResource->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                FRotator TargetRotation = UKismetMathLibrary::MakeRotFromX(Direction);
                SetActorRotation(TargetRotation);
                StartGoldMining(CurrentGoldResource); // Notify the blueprints of the gold actor object reference

                // Start mining
                bIsMining = true;
                UE_LOG(LogTemp, Log, TEXT("Unit %s started mining gold resource %s"), *GetName(), *CurrentGoldResource->GetName());
            }
			else
			{
				bIsMining = false; // Clear mining flag
			}
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
