#pragma once
#include "Camera/PlayerCameraManager.h"
#include "AdamuProjectPlayerCameraManager.generated.h"

UCLASS()
class AAdamuProjectPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    AAdamuProjectPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

    virtual void UpdateCamera(float DeltaTime) override;

    void MoveCamera(FVector2D MovementVector, float DeltaTime);
    void ZoomCamera(float ZoomValue);
    void RotateCameraLeft(float DeltaTime);
    void RotateCameraRight(float DeltaTime);

    bool bIsRotatingLeft = false;
    bool bIsRotatingRight = false;

private:
    void UpdateEdgeScrolling(float DeltaTime);

    FVector CameraLocation = FVector(0.0f, 400.0f, 968.0f); // Initial position
    FRotator CameraRotation = FRotator(-60.0f, 0.0f, 0.0f); // Initial rotation
    float CameraDistance = 700.0f;

    // Snap-back rotation properties
    float OriginalYaw = 0.0f; // Starting yaw to return to
    float SnapBackSpeed = 180.0f; // Degrees per second to snap back

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinZoom = 200.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxZoom = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float ZoomSpeed = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float RotationSpeed = 90.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float EdgeScrollThreshold = 10.0f;


};