// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AdamuProjectPlayerState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAdamuProjectPlayerState();

	// Team number for this player
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Ownership")
	int32 TeamNumber = 0;

	// Override to ensure replication works
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
