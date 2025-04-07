// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBaseUnit_Air.h"
#include "AIController.h"

ARTSBaseUnit_Air::ARTSBaseUnit_Air()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void ARTSBaseUnit_Air::SetMovementTarget(FVector NewTarget, AActor* TargetActor, EResourceType ResourceType)
{
	MovementTarget = NewTarget;
	bIsMoving = true;
	UE_LOG(LogTemp, Log, TEXT("Air Unit %s moving to: %s"), *GetName(), *MovementTarget.ToString());
}

void ARTSBaseUnit_Air::UpdateMovement(float DeltaTime)
{
	// TODO Placeholder for air movement
}

