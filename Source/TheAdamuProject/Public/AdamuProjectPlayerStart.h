// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "RTSBaseBuilding.h"
#include "RTSBaseUnit_Ground.h"
#include "AdamuProjectPlayerStart.generated.h"


UENUM(BlueprintType)
enum class ERaceType : uint8
{
	Human,
	Apkallu
};

USTRUCT(BlueprintType)
struct FPlayerStartRaceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Config")
	TSubclassOf<ARTSBaseBuilding> BuildingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Config")
	TSubclassOf<ARTSBaseUnit_Ground> GroundUnitClass;
};

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	AAdamuProjectPlayerStart(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "RTS")
	void SpawnMainBuildingAndWorkers();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Setup")
	FPlayerStartRaceConfig HumanConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Setup")
	FPlayerStartRaceConfig ApkalluConfig;

	// TODO: This will eventually be set by the players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race Setup")
	ERaceType SelectedRace;

	virtual void MovePlayerCameraToStartLocation();


private:
	bool IsSpawnLocationValid(const FVector& Location, FCollisionShape& OutCollisionShape) const;
};
