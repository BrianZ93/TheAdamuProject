// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AdamuProjectGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Starting    UMETA(DisplayName = "Starting"),
	Playing     UMETA(DisplayName = "Playing"),
	Ended       UMETA(DisplayName = "Ended")
};

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAdamuProjectGameStateBase();

	virtual void Tick(float DeltaTime) override;
	
	// Current phase of the game
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	EGamePhase CurrentPhase;

	// Optional: Total time the game has been running (in seconds)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	float GameTime;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
