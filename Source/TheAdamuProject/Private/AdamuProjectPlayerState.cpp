// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectPlayerState.h"
#include "Net/UnrealNetwork.h"

AAdamuProjectPlayerState::AAdamuProjectPlayerState()
{
}

void AAdamuProjectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register TeamNumber for replication
	DOREPLIFETIME(AAdamuProjectPlayerState, TeamNumber);
}
