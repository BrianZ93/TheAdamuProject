// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"
#include "Animation/AnimInstance.h"
#include "RTSBaseUnit.generated.h"

// Forward declaration instead of #include "AdamuProjectPlayerController.h" to avoid circular dependency
class AAdamuProjectPlayerController;

// Enum for resource types
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None    UMETA(DisplayName = "None"),
	Gold    UMETA(DisplayName = "Gold"),
	Stone   UMETA(DisplayName = "Stone"),
};

UCLASS(Abstract) // Mark as abstract since it’s a base class
class THEADAMUPROJECT_API ARTSBaseUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARTSBaseUnit();

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

	// Function to toggle decals based on selection state
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SetSelectionState(bool bIsSelected, bool bIsPlayerOwned, bool bIsProspect);

	// Virtual function to set the movement target
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void SetMovementTarget(FVector NewTarget, AActor* TargetActor = nullptr, EResourceType ResourceType = EResourceType::None);

	// Virtual function to update movement (called each frame)
	virtual void UpdateMovement(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Mining")
	virtual void StartGoldMining(AActor* GoldResource);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CustomMoveSpeed = 500.0f;

	// New Animation Blueprint property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TSubclassOf<UAnimInstance> UnitAnimationBlueprint;

	UFUNCTION(BlueprintCallable, Category = "Mining")
	AActor* GetCurrentGoldResource() const { return CurrentGoldResource; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Current movement target
	FVector MovementTarget;

	// Whether the unit is moving
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	bool bIsMoving;

	// New properties for mining behavior
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	bool bIsMining;

	// Current target actor (e.g., BP_Gold), restored as a UPROPERTY
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	AActor* CurrentGoldResource;

	// Current target actor (e.g., BP_Gold), restored as a UPROPERTY
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	EResourceType CurrentResourceType = EResourceType::None;
};
