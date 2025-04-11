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

    FVector GetCameraLocation() const { return CameraLocation; }

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraLocation(const FVector& NewLocation) { CameraLocation = NewLocation; }

    FRotator GetCameraRotation() const { return CameraRotation; }

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraRotation(const FRotator& NewRotation) { CameraRotation = NewRotation; }

    UFUNCTION(BlueprintCallable, Category = "Camera")
    float GetOriginalYaw() const { return OriginalYaw; }

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetOriginalYaw(float NewYaw) { OriginalYaw = NewYaw; }

private:
    void UpdateEdgeScrolling(float DeltaTime);

    FVector CameraLocation = FVector(0.0f, 400.0f, 968.0f); // Initial position
    FRotator CameraRotation = FRotator(-60.0f, 0.0f, 0.0f); // Initial rotation
    float CameraDistance = 500.0f;

    // Snap-back rotation properties
    float OriginalYaw = 0.0f; // Starting yaw to return to
    float SnapBackSpeed = 180.0f; // Degrees per second to snap back

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MinZoom =100.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxZoom = 1200.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float ZoomSpeed = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float RotationSpeed = 90.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float EdgeScrollThreshold = 10.0f;
};