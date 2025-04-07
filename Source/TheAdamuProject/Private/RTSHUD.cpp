// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSHUD.h"

ARTSHUD::ARTSHUD()
{
    bDrawSelectionRect = false;
    StartPos = FVector2D(0.0f, 0.0f);
    EndPos = FVector2D(0.0f, 0.0f);
}

void ARTSHUD::DrawHUD()
{
    Super::DrawHUD();

    if (bDrawSelectionRect)
    {
        float MinX = FMath::Min(StartPos.X, EndPos.X);
        float MaxX = FMath::Max(StartPos.X, EndPos.X);
        float MinY = FMath::Min(StartPos.Y, EndPos.Y);
        float MaxY = FMath::Max(StartPos.Y, EndPos.Y);

        float Width = MaxX - MinX;
        float Height = MaxY - MinY;

        DrawRect(FLinearColor(0, 1, 0, 0.5f), MinX, MinY, Width, Height);
    }
}
