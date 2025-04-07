// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSHUD.generated.h"

UCLASS(Blueprintable)
class THEADAMUPROJECT_API ARTSHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARTSHUD();

    virtual void DrawHUD() override;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    bool bDrawSelectionRect;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    FVector2D StartPos;

    UPROPERTY(BlueprintReadWrite, Category = "Selection")
    FVector2D EndPos;
};
