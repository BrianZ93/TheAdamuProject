// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_AIController.h"
#include "RTSBaseUnit.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"

ARTS_AIController::ARTS_AIController()
{
	// Ensure the PathFollowingComponent is created
	bWantsPlayerState = false;
}

void ARTS_AIController::BeginPlay()
{
    Super::BeginPlay();

    // Configure path following
    UPathFollowingComponent* PathComp = GetPathFollowingComponent();
    if (PathComp)
    {
        PathComp->OnRequestFinished.AddUObject(this, &ARTS_AIController::OnMoveCompleted);
        UE_LOG(LogTemp, Log, TEXT("RTS_AIController: PathFollowingComponent initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RTS_AIController: PathFollowingComponent not found"));
    }
}

//void ARTS_AIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//    ControlledUnit = Cast<ARTSBaseUnit>(InPawn);
//    if (!ControlledUnit)
//    {
//        UE_LOG(LogTemp, Error, TEXT("RTS_AIController possessed a non-RTSBaseUnit pawn!"));
//    }
//    else
//    {
//        UE_LOG(LogTemp, Log, TEXT("RTS_AIController possessed unit: %s"), *ControlledUnit->GetName());
//    }
//}

void ARTS_AIController::MoveToTargetLocation(FVector TargetLocation, float AcceptanceRadius)
{
    CurrentTargetLocation = TargetLocation;
    CurrentAcceptanceRadius = AcceptanceRadius;

    ACharacter* ControlledPawn = GetCharacter();
    if (!ControlledPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("RTS_AIController has no controlled pawn"));
        return;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("Navigation system not found"));
        return;
    }

    // Move to location using the navigation system
    EPathFollowingRequestResult::Type Result = MoveToLocation(TargetLocation, AcceptanceRadius, true, true, true, true);
    if (Result == EPathFollowingRequestResult::RequestSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("RTS_AIController issued move to %s"), *TargetLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RTS_AIController failed to issue move to %s"), *TargetLocation.ToString());
    }
}

void ARTS_AIController::StopUnitMovement()
{
    UPathFollowingComponent* PathComp = GetPathFollowingComponent();
    if (PathComp)
    {
        PathComp->AbortMove(*this, FPathFollowingResultFlags::ForcedScript, FAIRequestID::AnyRequest, EPathFollowingVelocityMode::Reset);
        UE_LOG(LogTemp, Log, TEXT("RTS_AIController stopped movement"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RTS_AIController: No PathFollowingComponent to stop movement"));
    }
}

void ARTS_AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (Result.IsSuccess())
    {
        UE_LOG(LogTemp, Log, TEXT("RTS_AIController successfully reached target"));
    }
    else if (Result.IsInterrupted())
    {
        UE_LOG(LogTemp, Log, TEXT("RTS_AIController movement was interrupted"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RTS_AIController failed to reach target with code: %d"), (int32)Result.Code);
    }
}