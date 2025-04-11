// Fill out your copyright notice in the Description page of Project Settings.


#include "AdamuProjectGameModeBase.h"
#include "AdamuProjectPlayerState.h"
#include "AdamuProjectPlayerController.h"
#include "AdamuProjectGameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "AdamuProjectPlayerStart.h"

AAdamuProjectGameModeBase::AAdamuProjectGameModeBase()
{
	// Set default classes
	PlayerStateClass = AAdamuProjectPlayerState::StaticClass();
	PlayerControllerClass = AAdamuProjectPlayerController::StaticClass();
    GameStateClass = AAdamuProjectGameStateBase::StaticClass();

	MaxPlayers = 12;
	CurrentGameMode = EGameModeType::Multiplayer;
	NextPlayerID = 0;

    // Initialize GameplayManager to nullptr
    GameplayManager = nullptr;
}

void AAdamuProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    // Count number of players
    int32 PlayerCount = GameState->PlayerArray.Num();
    UE_LOG(LogTemp, Log, TEXT("Number of Players Detected: %d"), PlayerCount);

    // Optional: Log game phase
    if (AAdamuProjectGameStateBase* AdamuGameState = Cast<AAdamuProjectGameStateBase>(GameState))
    {
        UE_LOG(LogTemp, Log, TEXT("Current Game Phase: %s"), *UEnum::GetValueAsString(AdamuGameState->CurrentPhase));
    }
}

void AAdamuProjectGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	// This function is run before any other actors are initialized

    // Spawn the RTSGameplayManager
    if (UWorld* World = GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        GameplayManager = World->SpawnActor<ARTSGameplayManager>(ARTSGameplayManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (GameplayManager)
        {
            UE_LOG(LogTemp, Log, TEXT("RTSGameplayManager spawned successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn RTSGameplayManager."));
        }
    }
}

void AAdamuProjectGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AAdamuProjectGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AAdamuProjectGameModeBase::AddNewPlayer(APlayerController* NewPlayerController)
{
    if (ActivePlayers.Num() >= MaxPlayers)
    {
        // Handle max players reached
        return;
    }

    // Cast to your custom classes
    AAdamuProjectPlayerController* AdamuController = Cast<AAdamuProjectPlayerController>(NewPlayerController);
    if (!AdamuController || !AdamuController->PlayerState)
    {
        return;
    }

    AAdamuProjectPlayerState* AdamuPlayerState = Cast<AAdamuProjectPlayerState>(AdamuController->PlayerState);
    if (!AdamuPlayerState)
    {
        return;
    }

    // Set up player state
    //AdamuPlayerState->Score = 0; // Example property from APlayerState, adjust as needed
    //AdamuPlayerState->PlayerName = "Player_" + FString::FromInt(NextPlayerID);

    // Add to ActivePlayers if you still need additional tracking
    FPlayerData NewPlayer;
    NewPlayer.PlayerID = NextPlayerID++;
    NewPlayer.TeamNumber = 0; // Default team, adjust as needed
    ActivePlayers.Add(NewPlayer);
	UE_LOG(LogTemp, Log, TEXT("Player %d added to team %d"), NewPlayer.PlayerID, NewPlayer.TeamNumber);
}

AAdamuProjectPlayerState* AAdamuProjectGameModeBase::GetPlayerStateByID(int32 PlayerID)
{
    //for (APlayerState* State : GameState->PlayerArray) // Uses default GameStateBase
    //{
    //    if (AAdamuProjectPlayerState* AdamuState = Cast<AAdamuProjectPlayerState>(State))
    //    {
    //        if (AdamuState->PlayerID == PlayerID)
    //        {
    //            return AdamuState;
    //        }
    //    }
    //}
    return nullptr;
}

AAdamuProjectPlayerState* AAdamuProjectGameModeBase::GetNextPlayerForSpawn()
{
    // If single player, just return first player
    if (GameState->PlayerArray.Num() == 1)
    {
        return Cast<AAdamuProjectPlayerState>(GameState->PlayerArray[0]);
    }

    // For multiplayer, get the next player based on NextPlayerID
    for (APlayerState* State : GameState->PlayerArray)
    {
        if (AAdamuProjectPlayerState* AdamuState = Cast<AAdamuProjectPlayerState>(State))
        {
            // Note: This assumes your PlayerState has a GetPlayerId() function
            // You might need to adjust this based on how you track player IDs
            if (AdamuState->GetPlayerId() == NextPlayerID)
            {
                return AdamuState;
            }
        }
    }

    // If no matching player found, return nullptr
    return nullptr;
}
