// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "AdamuProjectPlayerStart.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	AAdamuProjectPlayerStart(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCapsuleComponent* CollisionCapsule;
};
