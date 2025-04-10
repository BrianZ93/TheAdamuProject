#include "AdamuProjectPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

AAdamuProjectPlayerCameraManager::AAdamuProjectPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Ensure Tick is called
    OriginalYaw = CameraRotation.Yaw; // Set initial yaw as the original
}

void AAdamuProjectPlayerCameraManager::UpdateCamera(float DeltaTime)
{
    Super::UpdateCamera(DeltaTime);

    // Apply rotation if active
    if (bIsRotatingLeft)
    {
        RotateCameraLeft(DeltaTime);
    }
    if (bIsRotatingRight)
    {
        RotateCameraRight(DeltaTime);
    }

    // Snap back to original yaw when not rotating
    if (!bIsRotatingLeft && !bIsRotatingRight && !FMath::IsNearlyEqual(CameraRotation.Yaw, OriginalYaw, 0.1f))
    {
        float DeltaRotation = FMath::Sign(OriginalYaw - CameraRotation.Yaw) * SnapBackSpeed * DeltaTime;
        CameraRotation.Yaw += DeltaRotation;

        // Clamp to prevent overshooting
        if (FMath::Sign(OriginalYaw - CameraRotation.Yaw) != FMath::Sign(OriginalYaw - (CameraRotation.Yaw + DeltaRotation)))
        {
            CameraRotation.Yaw = OriginalYaw;
        }

    }

    // Ensure pitch remains at -90 for top-down view
    CameraRotation.Pitch = -90.0f;

    // Update edge scrolling
    UpdateEdgeScrolling(DeltaTime);

    // Calculate final camera position
    FVector CameraOffset = CameraRotation.Vector() * -CameraDistance;
    FVector FinalLocation = CameraLocation + CameraOffset;

    // Set camera POV
    FMinimalViewInfo ViewInfo;
    ViewInfo.Location = FinalLocation;
    ViewInfo.Rotation = CameraRotation;
    SetCameraCachePOV(ViewInfo);
}

void AAdamuProjectPlayerCameraManager::MoveCamera(FVector2D MovementVector, float DeltaTime)
{
    FVector MovementDelta(MovementVector.X * CameraSpeed * DeltaTime, MovementVector.Y * CameraSpeed * DeltaTime, 0.0f);
    CameraLocation += MovementDelta;
}

void AAdamuProjectPlayerCameraManager::ZoomCamera(float ZoomValue)
{
    CameraDistance = FMath::Clamp(CameraDistance - ZoomValue * ZoomSpeed, MinZoom, MaxZoom);
}

void AAdamuProjectPlayerCameraManager::RotateCameraLeft(float DeltaTime)
{
    CameraRotation.Yaw -= RotationSpeed * DeltaTime;
}

void AAdamuProjectPlayerCameraManager::RotateCameraRight(float DeltaTime)
{
    CameraRotation.Yaw += RotationSpeed * DeltaTime;
}

void AAdamuProjectPlayerCameraManager::UpdateEdgeScrolling(float DeltaTime)
{
    FVector2D MousePosition;
    if (PCOwner && PCOwner->GetMousePosition(MousePosition.X, MousePosition.Y))
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);

        FVector MovementDelta(0.0f, 0.0f, 0.0f);
        if (MousePosition.X < EdgeScrollThreshold)
            MovementDelta.Y = -CameraSpeed * DeltaTime; // Left
        else if (MousePosition.X > ViewportSize.X - EdgeScrollThreshold)
            MovementDelta.Y = CameraSpeed * DeltaTime;  // Right
        if (MousePosition.Y < EdgeScrollThreshold)
            MovementDelta.X = CameraSpeed * DeltaTime;  // Up
        else if (MousePosition.Y > ViewportSize.Y - EdgeScrollThreshold)
            MovementDelta.X = -CameraSpeed * DeltaTime; // Down

        if (MovementDelta != FVector::ZeroVector)
        {
            CameraLocation += MovementDelta;
        }
    }
}