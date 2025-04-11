// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTS_AIController.generated.h"

/**
 * 
 */
UCLASS()
class THEADAMUPROJECT_API ARTS_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
    ARTS_AIController();

    virtual void BeginPlay() override;

    //virtual void OnPossess(APawn* InPawn) override;

    // Move to a target location with an acceptance radius
    void MoveToTargetLocation(FVector TargetLocation, float AcceptanceRadius);

    // Stop movement cleanly
    void StopUnitMovement();

protected:
    // Handle move completed event
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    // Store the current target location
    FVector CurrentTargetLocation;

    // Acceptance radius for reaching the target
    float CurrentAcceptanceRadius;
};
