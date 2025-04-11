// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AdamuProjectPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Active      UMETA(DisplayName = "Active"),
	Defeated    UMETA(DisplayName = "Defeated"),
	Spectating  UMETA(DisplayName = "Spectating")
};

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAdamuProjectPlayerState();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	EPlayerState CurrentState;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	int32 TeamNumber;

	// Override to ensure replication works
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
