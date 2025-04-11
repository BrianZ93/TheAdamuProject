// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSBaseUnit.h"
#include "RTSResourceInteractionInterface.h"
#include "RTSBaseUnit_Ground.generated.h"

class UBP_ResourceManagerComponent;

UCLASS(Blueprintable)
class THEADAMUPROJECT_API ARTSBaseUnit_Ground : public ARTSBaseUnit, public IRTSResourceInteractionInterface
{
	GENERATED_BODY()

public:
	ARTSBaseUnit_Ground();

	// Override movement functions
	virtual void SetMovementTarget(FVector NewTarget, AActor* TargetActor = nullptr, EResourceType ResourceType = EResourceType::None);
	virtual void UpdateMovement(float DeltaTime) override;
	virtual void StartGoldMining(AActor* GoldResource) override;

	// New method to trigger drop-off, callable from Blueprint
	UFUNCTION(BlueprintCallable, Category = "Mining")
	void StopMiningAndDropoffGold();

	// Existing method to find drop-off and deposit gold
	UFUNCTION(BlueprintCallable, Category = "Mining")
	virtual void FindNearestGoldDepositLocation_Implementation() override;

protected:
	// Init here as well as base class
	virtual void BeginPlay() override;

	// Controller for pathfinding
	class ARTS_AIController* AIController;

	// Flag to track if we're waiting for a pathfinding result
	bool bIsPathfinding;
};
