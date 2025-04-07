// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSBaseUnit.h"
#include "RTSBaseUnit_Air.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API ARTSBaseUnit_Air : public ARTSBaseUnit
{
	GENERATED_BODY()

public:
	ARTSBaseUnit_Air();

	// Override movement functions
	virtual void SetMovementTarget(FVector NewTarget, AActor* TargetActor = nullptr, EResourceType ResourceType = EResourceType::None);
	virtual void UpdateMovement(float DeltaTime) override;
};
