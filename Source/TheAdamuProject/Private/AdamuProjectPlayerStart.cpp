// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectPlayerStart.h"
#include "Components/CapsuleComponent.h"

AAdamuProjectPlayerStart::AAdamuProjectPlayerStart(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer) // Chain to APlayerStart's constructor
{
    // Get the existing capsule component (inherited from APlayerStart)
    CollisionCapsule = Cast<UCapsuleComponent>(GetRootComponent());
    if (CollisionCapsule)
    {
        // Disable collision or overlap events to prevent interference
        CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionCapsule->SetGenerateOverlapEvents(false);

        // Optional: Adjust collision settings if you want some interaction
        // CollisionCapsule->SetCollisionProfileName(TEXT("NoCollision"));
    }
    else
    {
        // Fallback: Create a new capsule component if needed
        CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
        CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CollisionCapsule->SetGenerateOverlapEvents(false);
        SetRootComponent(CollisionCapsule);
    }
}

void AAdamuProjectPlayerStart::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("RTSPlayerStart BeginPlay called"));

    // TODO add logic to start the players characters here before destroying player start
    Destroy();
}
