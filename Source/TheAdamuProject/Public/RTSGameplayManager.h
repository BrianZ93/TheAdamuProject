// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSGameplayManager.generated.h"

UCLASS(Blueprintable)
class THEADAMUPROJECT_API ARTSGameplayManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ARTSGameplayManager();

	// Function to spawn an actor
	UFUNCTION(BlueprintCallable, Category = "RTS")
	AActor* SpawnActorAndAddToPool(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, APlayerController* PlayerController); 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Rebuilds the navmesh around the specified location with a given extent
	void RebuildNavMeshAroundLocation(const FVector& Location, const FVector& Extent);
};
