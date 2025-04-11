// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RTSBaseUnit.h"
#include "RTSResourceInteractionInterface.generated.h"

UINTERFACE(Blueprintable)
class URTSResourceInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class THEADAMUPROJECT_API IRTSResourceInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	IRTSResourceInteractionInterface();

    // Checks if this is a gold resource
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
    bool IsGoldResource() const;

    // Called when a gold resource is right-clicked, passing selected units
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
    bool OnGoldResourceRightClicked(const TArray<ARTSBaseUnit*>& SelectedUnits);

    // Find nearest gold deposit
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
    void FindNearestGoldDepositLocation();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resource")
    bool BuildingCanReceiveGold();
};
