#include "AdamuProjectPlayerController.h"
#include "AdamuProjectPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RTSBaseUnit_Ground.h"
#include "RTSBaseUnit_Air.h"
#include "RTSBaseBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "RTSHUD.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "AdamuProjectPlayerCameraManager.h"
#include "RTSResourceInteractionInterface.h"
#include "Components/ActorComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

AAdamuProjectPlayerController::AAdamuProjectPlayerController()
{
    RTSHUD = ARTSHUD::StaticClass();
    CurrentMovementVector = FVector2D(0.0f, 0.0f);
    CurrentRotationAngle = 0.0f;
    OriginalRotationAngle = 0.0f;
    bIsRotatingLeft = false;
    bIsRotatingRight = false;
    bIsDraggingSelection = false;

    PrimaryActorTick.bCanEverTick = true;

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    InputMappingContext = nullptr;
    MoveUpAction = nullptr;
    MoveDownAction = nullptr;
    MoveRightAction = nullptr;
    MoveLeftAction = nullptr;
    ZoomAction = nullptr;
    RotateLeftAction = nullptr;
    RotateRightAction = nullptr;
    LeftClickAction = nullptr;
    RightClickAction = nullptr;

    // Set the custom CameraManager
    PlayerCameraManagerClass = AAdamuProjectPlayerCameraManager::StaticClass();
}

void AAdamuProjectPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AAdamuProjectPlayerController, ProspectiveSelectedUnits);
    DOREPLIFETIME(AAdamuProjectPlayerController, SelectedUnits);
}

AAdamuProjectPlayerState* AAdamuProjectPlayerController::GetAdamuPlayerState() const
{
    return Cast<AAdamuProjectPlayerState>(PlayerState);
}

void AAdamuProjectPlayerController::SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime, EViewTargetBlendFunction BlendFunc, float BlendExp, bool bLockOutgoing)
{
    // TODO Remove this override if CameraManager handles everything
    Super::SetViewTargetWithBlend(NewViewTarget, BlendTime, BlendFunc, BlendExp, bLockOutgoing);
}

void AAdamuProjectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (InputMappingContext)
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }

    bShowMouseCursor = true;
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);

    UE_LOG(LogTemp, Log, TEXT("Input mode set. ShowMouseCursor: %d"), bShowMouseCursor);
}

void AAdamuProjectPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update HUD (unchanged)
    if (ARTSHUD* RTSHudInstance = Cast<ARTSHUD>(GetHUD()))
    {
        RTSHudInstance->bDrawSelectionRect = bIsDraggingSelection;
        RTSHudInstance->StartPos = SelectionStartPos;
        RTSHudInstance->EndPos = SelectionEndPos;
    }
}

void AAdamuProjectPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (MoveUpAction)
        {
            EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::MoveUp);
            EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::MoveUp);
        }
        if (MoveDownAction)
        {
            EnhancedInputComponent->BindAction(MoveDownAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::MoveDown);
            EnhancedInputComponent->BindAction(MoveDownAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::MoveDown);
        }
        if (MoveRightAction)
        {
            EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::MoveRight);
            EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::MoveRight);
        }
        if (MoveLeftAction)
        {
            EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::MoveLeft);
            EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::MoveLeft);
        }
        if (ZoomAction)
        {
            EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::Zoom);
        }
        if (RotateLeftAction)
        {
            EnhancedInputComponent->BindAction(RotateLeftAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::RotateLeft);
            EnhancedInputComponent->BindAction(RotateLeftAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::RotateLeft);
        }
        if (RotateRightAction)
        {
            EnhancedInputComponent->BindAction(RotateRightAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::RotateRight);
            EnhancedInputComponent->BindAction(RotateRightAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::RotateRight);
        }
        if (LeftClickAction)
        {
            EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &AAdamuProjectPlayerController::OnLeftClickStarted);
            EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::OnLeftClickOngoing);
            EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::OnLeftClickReleased);
        }
        if (RightClickAction)
        {
            EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Triggered, this, &AAdamuProjectPlayerController::OnRightClick);
            EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &AAdamuProjectPlayerController::OnRightClickReleased);
        }
    }
}

void AAdamuProjectPlayerController::MoveUp(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    CurrentMovementVector.X = bIsPressed ? 1.0f : 0.0f;
    ApplyMovement();
}

void AAdamuProjectPlayerController::MoveDown(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    CurrentMovementVector.X = bIsPressed ? -1.0f : 0.0f;
    ApplyMovement();
}

void AAdamuProjectPlayerController::MoveRight(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    CurrentMovementVector.Y = bIsPressed ? 1.0f : 0.0f;
    ApplyMovement();
}

void AAdamuProjectPlayerController::MoveLeft(const FInputActionValue& Value)
{
    bool bIsPressed = Value.Get<bool>();
    CurrentMovementVector.Y = bIsPressed ? -1.0f : 0.0f;
    ApplyMovement();
}

void AAdamuProjectPlayerController::ApplyMovement()
{
    if (AAdamuProjectPlayerCameraManager* CameraManager = Cast<AAdamuProjectPlayerCameraManager>(PlayerCameraManager))
    {
        CameraManager->MoveCamera(CurrentMovementVector, GetWorld()->GetDeltaSeconds());
    }
}

void AAdamuProjectPlayerController::Zoom(const FInputActionValue& Value)
{
    if (AAdamuProjectPlayerCameraManager* CameraManager = Cast<AAdamuProjectPlayerCameraManager>(PlayerCameraManager))
    {
        CameraManager->ZoomCamera(Value.Get<float>());
    }
}

void AAdamuProjectPlayerController::RotateLeft(const FInputActionValue& Value)
{
    if (AAdamuProjectPlayerCameraManager* CameraManager = Cast<AAdamuProjectPlayerCameraManager>(PlayerCameraManager))
    {
        bool bIsPressed = Value.Get<bool>();
        CameraManager->bIsRotatingLeft = bIsPressed;
    }
}

void AAdamuProjectPlayerController::RotateRight(const FInputActionValue& Value)
{
    if (AAdamuProjectPlayerCameraManager* CameraManager = Cast<AAdamuProjectPlayerCameraManager>(PlayerCameraManager))
    {
        bool bIsPressed = Value.Get<bool>();
        CameraManager->bIsRotatingRight = bIsPressed;
    }
}

// Mouse input functions
void AAdamuProjectPlayerController::OnLeftClickStarted(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        FVector2D MousePos;
        bool bGotMousePos = GetMousePosition(MousePos.X, MousePos.Y);

        SelectionStartPos = MousePos;
        bIsDraggingSelection = false;
    }
}

void AAdamuProjectPlayerController::OnLeftClickOngoing(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        FVector2D MousePos;
        bool bGotMousePos = GetMousePosition(MousePos.X, MousePos.Y);

        if (bFirstDragFrame)
        {
            SelectionStartPos = MousePos;
            bFirstDragFrame = false;
        }

        SelectionEndPos = MousePos;
        float Distance = FVector2D::Distance(SelectionStartPos, SelectionEndPos);

        if (Distance > 5.0f)
        {
            bIsDraggingSelection = true;
            TArray<ARTSBaseUnit*> PreviousProspectiveUnits = ProspectiveSelectedUnits;
            ProspectiveSelectedUnits.Empty();

            TArray<AActor*> ActorsInLevel;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSBaseUnit::StaticClass(), ActorsInLevel);

            AAdamuProjectPlayerState* MyPlayerState = Cast<AAdamuProjectPlayerState>(PlayerState);
            if (!MyPlayerState)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerState to AAdamuProjectPlayerState"));
                return;
            }

            for (AActor* Actor : ActorsInLevel)
            {
                if (ARTSBaseUnit* Unit = Cast<ARTSBaseUnit>(Actor))
                {
                    FVector2D UnitScreenPos;
                    ProjectWorldLocationToScreen(Unit->GetActorLocation(), UnitScreenPos);

                    float MinX = FMath::Min(SelectionStartPos.X, SelectionEndPos.X);
                    float MaxX = FMath::Max(SelectionStartPos.X, SelectionEndPos.X);
                    float MinY = FMath::Min(SelectionStartPos.Y, SelectionEndPos.Y);
                    float MaxY = FMath::Max(SelectionStartPos.Y, SelectionEndPos.Y);

                    bool bIsInRectangle = (UnitScreenPos.X >= MinX && UnitScreenPos.X <= MaxX &&
                        UnitScreenPos.Y >= MinY && UnitScreenPos.Y <= MaxY);

                    if (bIsInRectangle)
                    {
                        ProspectiveSelectedUnits.Add(Unit);
                        Unit->SetSelectionState(true, true, false);
                    }
                    else if (PreviousProspectiveUnits.Contains(Unit))
                    {
                        // Deselect units that were in the previous prospective selection
                        Unit->SetSelectionState(false, false, false);
                    }
                }
            }
        }
    }
}

void AAdamuProjectPlayerController::OnLeftClickReleased(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())

    {
        FVector2D MousePos;
        GetMousePosition(MousePos.X, MousePos.Y);
        SelectionEndPos = MousePos;

        AAdamuProjectPlayerState* MyPlayerState = Cast<AAdamuProjectPlayerState>(PlayerState);
        if (!MyPlayerState)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to cast PlayerState to AAdamuProjectPlayerState"));
            return;
        }

        // Deselect all currently selected units before applying new selection
        for (ARTSBaseUnit* Unit : SelectedUnits)
        {
            Unit->SetSelectionState(false, false, false);
        }

        if (bIsDraggingSelection)
        {
            SelectedUnits = ProspectiveSelectedUnits; // Sync with prospective
            UE_LOG(LogTemp, Log, TEXT("Drag selection completed. SelectedUnits count: %d"), SelectedUnits.Num());
            for (ARTSBaseUnit* Unit : SelectedUnits)
            {
                UE_LOG(LogTemp, Log, TEXT("Selected unit after drag: %s"), *Unit->GetName());
                Unit->SetSelectionState(true, true, false);
            }
            ProspectiveSelectedUnits.Empty();
        }
        else
        {
            FHitResult Hit;
            GetHitResultUnderCursor(ECC_Visibility, true, Hit);
            if (Hit.bBlockingHit)
            {
                if (ARTSBaseUnit* Unit = Cast<ARTSBaseUnit>(Hit.GetActor()))
                {
                    if (Unit->OwningPlayerId == MyPlayerState->GetPlayerId())
                    {
                        SelectedUnits.Empty();
                        SelectedUnits.Add(Unit);
                        Unit->SetSelectionState(true, true, false);
                        UE_LOG(LogTemp, Log, TEXT("Selected Unit: %s"), *Unit->GetName());
                    }
                }
                else
                {
                    // If we hit the terrain (or anything that's not a unit), clear selection
                    SelectedUnits.Empty();
                    UE_LOG(LogTemp, Log, TEXT("Clicked on terrain, deselected all units"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No valid hit result under cursor for selection."));
                SelectedUnits.Empty();
            }
        }

        UE_LOG(LogTemp, Log, TEXT("After selection: SelectedUnits count: %d"), SelectedUnits.Num());
        bIsDraggingSelection = false;
        SelectionStartPos = FVector2D(0.0f, 0.0f);
        SelectionEndPos = FVector2D(0.0f, 0.0f);
        bFirstDragFrame = true;
    }
}

void AAdamuProjectPlayerController::HandleRightClickOnUnit(AActor* HitActor, FVector HitLocation)
{
    if (!IsValid(HitActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRightClickOnUnit called with invalid HitActor"));
        return;
    }

    ARTSBaseUnit* HitUnit = Cast<ARTSBaseUnit>(HitActor);
    ARTSBaseBuilding* HitBuilding = Cast<ARTSBaseBuilding>(HitActor);

    if (HitUnit)
    {
        UE_LOG(LogTemp, Log, TEXT("Right-clicked on RTSBaseUnit: %s"), *HitUnit->GetName());
    }
    else if (HitBuilding)
    {
        UE_LOG(LogTemp, Log, TEXT("Right-clicked on a building: %s"), *HitBuilding->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRightClickOnUnit called with invalid actor (not a unit or building): %s"), *HitActor->GetName());
        return;
    }

    SpawnRightClickEffect(HitLocation);

    if (SelectedUnits.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Attempting to move to target: %s"), *HitActor->GetName());
        UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavSystem)
        {
            FNavLocation ProjectedLocation;
            bool bFoundValidLocation = NavSystem->ProjectPointToNavigation(HitLocation, ProjectedLocation, FVector(200.0f, 200.0f, 100.0f));
            FVector TargetLocation = bFoundValidLocation ? ProjectedLocation.Location : HitLocation;

            for (ARTSBaseUnit* Unit : SelectedUnits)
            {
                if (Unit)
                {
                    Unit->SetMovementTarget(TargetLocation, HitActor, EResourceType::None);
                }
            }
        }
    }
}

void AAdamuProjectPlayerController::HandleRightClickOnResource(AActor* HitActor, FVector HitLocation)
{
    if (IRTSResourceInteractionInterface::Execute_IsGoldResource(HitActor))
    {
        UE_LOG(LogTemp, Log, TEXT("Right-clicked on a gold resource at: %s"), *HitLocation.ToString());

        bool bIsGoldInteraction = IRTSResourceInteractionInterface::Execute_OnGoldResourceRightClicked(HitActor, SelectedUnits);
        UE_LOG(LogTemp, Log, TEXT("OnGoldResourceRightClicked returned: %s"), bIsGoldInteraction ? TEXT("true") : TEXT("false"));
        SpawnRightClickEffect(HitLocation);

        if (SelectedUnits.Num() > 0)
        {
            UE_LOG(LogTemp, Log, TEXT("Attempting to move to gold"));
            UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
            if (NavSystem)
            {
                FNavLocation ProjectedLocation;
                bool bFoundValidLocation = NavSystem->ProjectPointToNavigation(HitLocation, ProjectedLocation, FVector(200.0f, 200.0f, 100.0f));
                FVector TargetLocation = bFoundValidLocation ? ProjectedLocation.Location : HitLocation;

                for (ARTSBaseUnit* Unit : SelectedUnits)
                {
                    if (Unit)
                    {
                        Unit->SetMovementTarget(TargetLocation, HitActor, EResourceType::Gold);
                    }
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Right-clicked on a non-gold resource at: %s"), *HitLocation.ToString());
        SpawnRightClickEffect(HitLocation);
    }
}

void AAdamuProjectPlayerController::HandleRightClickOnEmptySpace(FVector HitLocation)
{
    // Scenario 3: Hit terrain/empty space
    UE_LOG(LogTemp, Log, TEXT("Right-clicked on empty space at: %s"), *HitLocation.ToString());

    // Spawn the effect at the clicked location
    SpawnRightClickEffect(HitLocation);

    // Trigger movement for all selected units
    if (SelectedUnits.Num() > 0)
    {
        for (ARTSBaseUnit* Unit : SelectedUnits)
        {
            if (Unit)
            {
                Unit->SetMovementTarget(HitLocation, nullptr, EResourceType::None);
            }
        }
    }
}

void AAdamuProjectPlayerController::OnRightClick(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && !bIsRightClickLocked)
    {
        bIsRightClickLocked = true;

        FHitResult Hit;
        if (GetHitResultUnderCursor(ECC_WorldStatic, true, Hit) && Hit.bBlockingHit)
        {
            UE_LOG(LogTemp, Log, TEXT("Hit Location: %s"), *Hit.Location.ToString());

            AActor* HitActor = Hit.GetActor();
            if (!IsValid(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("HitActor is invalid after raycast"));
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("HitActor class: %s, Name: %s"), *HitActor->GetClass()->GetName(), *HitActor->GetName());

            ARTSBaseUnit* HitUnit = Cast<ARTSBaseUnit>(HitActor);
            ARTSBaseBuilding* HitBuilding = Cast<ARTSBaseBuilding>(HitActor);

            if (HitUnit || HitBuilding)
            {
                HandleRightClickOnUnit(HitActor, Hit.Location);
            }
            else if (HitActor && HitActor->Implements<URTSResourceInteractionInterface>())
            {
                HandleRightClickOnResource(HitActor, Hit.Location);
            }
            else
            {
                HandleRightClickOnEmptySpace(Hit.Location);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No valid hit under cursor"));
        }
    }
}

void AAdamuProjectPlayerController::SpawnRightClickEffect_Implementation(FVector Location)
{
    UE_LOG(LogTemp, Log, TEXT("C++ SpawnRightClickEffect_Implementation called"));
    if (RightClickEffect)
    {
        UE_LOG(LogTemp, Log, TEXT("Niagara Spawn Location: %s"), *Location.ToString());
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            RightClickEffect,
            Location,
            FRotator::ZeroRotator, // No rotation; Blueprint will handle it
            FVector(1.f, 1.f, 1.f),
            true,
            true
        );
    }
}

void AAdamuProjectPlayerController::OnRightClickReleased(const FInputActionValue& Value)
{
    bIsRightClickLocked = false; // Unlock the right-click action when the button is released
    UE_LOG(LogTemp, Log, TEXT("Right-click released, action unlocked"));
}
