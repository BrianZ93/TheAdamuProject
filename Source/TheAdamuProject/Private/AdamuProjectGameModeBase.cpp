// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectGameModeBase.h"
#include "AdamuProjectPlayerState.h"
#include "AdamuProjectPlayerController.h"

AAdamuProjectGameModeBase::AAdamuProjectGameModeBase()
{
	PlayerStateClass = AAdamuProjectPlayerState::StaticClass();
	PlayerControllerClass = AAdamuProjectPlayerController::StaticClass();
}
