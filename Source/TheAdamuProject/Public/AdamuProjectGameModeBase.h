// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AdamuProjectPlayerController.h"
#include "AdamuProjectPlayerState.h"
#include "AdamuProjectGameStateBase.h"
#include "RTSGameplayManager.h"
#include "AdamuProjectGameModeBase.generated.h"

UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	Campaign    UMETA(DisplayName = "Campaign"),
	Multiplayer UMETA(DisplayName = "Multiplayer")
};

USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 PlayerID;

    UPROPERTY(BlueprintReadWrite)
    int32 TeamNumber;

    FPlayerData()
    {
        PlayerID = -1;
        TeamNumber = 0;
    }
};

/**
 * 
 */
UCLASS(Blueprintable)
class THEADAMUPROJECT_API AAdamuProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAdamuProjectGameModeBase();

    // Called when the game starts
	virtual void BeginPlay() override;

    // Handle new player joining
	virtual void PostLogin(APlayerController* NewPlayer) override;

    // Handle player leaving
	virtual void Logout(AController* Exiting) override;

    // Game mode properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    EGameModeType CurrentGameMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 MaxPlayers;

    // Array to store additional player data (if needed beyond PlayerState)
    UPROPERTY(BlueprintReadOnly, Category = "Player Management")
    TArray<FPlayerData> ActivePlayers;

    // Reference to the RTSGameplayManager instance
    UPROPERTY(BlueprintReadOnly, Category = "Gameplay Manager")
    ARTSGameplayManager* GameplayManager;

    // Functions to manage players
    UFUNCTION(BlueprintCallable, Category = "Player Management")
    AAdamuProjectPlayerState* GetPlayerStateByID(int32 PlayerID);

    AAdamuProjectPlayerState* GetNextPlayerForSpawn();

protected:
    // Initialize game mode settings
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

private:
	// Internal counter for player IDs
    int32 NextPlayerID;

    // Helper function to add new player
    void AddNewPlayer(APlayerController* NewPlayerController);
};
