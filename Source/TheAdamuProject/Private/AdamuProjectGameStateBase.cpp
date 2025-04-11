// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectGameStateBase.h"
#include "Net/UnrealNetwork.h"

AAdamuProjectGameStateBase::AAdamuProjectGameStateBase()
{
    CurrentPhase = EGamePhase::Starting;
    GameTime = 0.0f;
}

void AAdamuProjectGameStateBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAdamuProjectGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAdamuProjectGameStateBase, CurrentPhase);
    DOREPLIFETIME(AAdamuProjectGameStateBase, GameTime);
}
