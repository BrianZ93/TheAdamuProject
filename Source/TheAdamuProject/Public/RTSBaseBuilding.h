// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "RTSResourceInteractionInterface.h"
#include "RTSBaseBuilding.generated.h"


class AAdamuProjectPlayerController;

UCLASS(Blueprintable)
class THEADAMUPROJECT_API ARTSBaseBuilding : public APawn, public IRTSResourceInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARTSBaseBuilding();

	// Player state & Controller attributes
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ownership")
	int32 OwningPlayerId = -1; // -1 means unowned

	// Team number, synced with owning player's TeamNumber
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ownership")
	int32 TeamNumber = -1;

	// Local reference to controller (not replicated)
	UPROPERTY(BlueprintReadWrite, Category = "Ownership")
	AAdamuProjectPlayerController* OwningController = nullptr;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Decal components for selection states
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Selection")
	UDecalComponent* SelectionDecal_Enemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Selection")
	UDecalComponent* SelectionDecal_PlayerOwned;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Selection")
	UDecalComponent* SelectionDecal_Prospect;

	// Static Mesh Component for the building's visual and collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BuildingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	// Function to toggle decals based on selection state
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SetSelectionState(bool bIsSelected, bool bIsPlayerOwned, bool bIsProspect);

	UFUNCTION(BlueprintCallable, Category = "Mining")
	virtual bool BuildingCanReceiveGold_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
