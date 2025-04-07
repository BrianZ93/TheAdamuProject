#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "RTSBaseUnit.h"
#include "RTSBaseUnit_ground.h"
#include "RTSHUD.h"
#include "AdamuProjectPlayerController.generated.h"

class ARTSBaseUnit;
class ARTSHUD;

UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAdamuProjectPlayerController();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    virtual void SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime = 0.f, EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0.f, bool bLockOutgoing = false) override;

protected:
    virtual void SetupInputComponent() override;

    // Movement and rotation functions
    void MoveUp(const FInputActionValue& Value);
    void MoveDown(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void MoveLeft(const FInputActionValue& Value);
    void ApplyMovement();
    void Zoom(const FInputActionValue& Value);
    void RotateLeft(const FInputActionValue& Value);
    void RotateRight(const FInputActionValue& Value);

    // Mouse input functions
    UFUNCTION()
    void OnLeftClickStarted(const FInputActionValue& Value);
    UFUNCTION()
    void OnLeftClickOngoing(const FInputActionValue& Value);
    UFUNCTION()
    void OnLeftClickReleased(const FInputActionValue& Value);
    UFUNCTION()
    void OnRightClick(const FInputActionValue& Value);

    void HandleRightClickOnUnit(ARTSBaseUnit* HitUnit, FVector HitLocation);
    void HandleRightClickOnResource(AActor* HitActor, FVector HitLocation);
    void HandleRightClickOnEmptySpace(FVector HitLocation);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* MoveUpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* MoveDownAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* MoveRightAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* MoveLeftAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* RotateLeftAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* RotateRightAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* LeftClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    class UInputAction* RightClickAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (ToolTip = "Niagara System to spawn when issuing a move command"))
    UNiagaraSystem* RightClickEffect;

    UFUNCTION(BlueprintNativeEvent, Category = "RTS")
    void SpawnRightClickEffect(FVector Location);
    virtual void SpawnRightClickEffect_Implementation(FVector Location);

    void OnRightClickReleased(const FInputActionValue& Value);

    FVector2D CurrentMovementVector;
    float CurrentRotationAngle;
    float OriginalRotationAngle;
    bool bIsRotatingLeft;
    bool bIsRotatingRight;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float SnapBackSpeed = 180.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxRotationAngle = 90.0f;

    // Unit selection
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Units")
    TArray<ARTSBaseUnit*> SelectedUnits;

    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Units")
    TArray<ARTSBaseUnit*> ProspectiveSelectedUnits;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Selection rectangle logic
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
    bool bIsDraggingSelection;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
    FVector2D SelectionStartPos;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
    FVector2D SelectionEndPos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<class ARTSHUD> RTSHUD;

private:
    bool bFirstDragFrame = true;

    bool bIsRightClickLocked = false; // Tracks if right-click is currently locked
};