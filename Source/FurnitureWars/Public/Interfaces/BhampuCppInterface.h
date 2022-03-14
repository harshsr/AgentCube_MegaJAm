// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "BhampuCppInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBhampuCppInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FURNITUREWARS_API IBhampuCppInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Get tag names functions
	UFUNCTION()
	virtual FName GetLongitudinalMoveTagName() = 0;
	UFUNCTION()
	virtual FName GetLateralMoveTagName() = 0;
	UFUNCTION()
	virtual FName GetYawMoveTagName() = 0;
	UFUNCTION()
	virtual FName GetCameraPitchTagName() = 0;
	UFUNCTION()
	virtual FName GetJumpTagName() = 0;
	UFUNCTION()
	virtual FName GetDashTagName() = 0;
	UFUNCTION()
	virtual FName GetStealthTagName() = 0;
	UFUNCTION()
	virtual FName GetInteractionAvailableTagName() = 0;

	// Get container reference
	UFUNCTION()
	virtual FGameplayTagContainer& GetBhampuTags() = 0;

	// Set axis value
	UFUNCTION()
	virtual void SetForwardAxisVal(float AxisValue) = 0;

	UFUNCTION()
	virtual void SetRightAxisVal(float AxisValue) = 0;

	UFUNCTION()
	virtual void SetYawAxisVal(float AxisValue) = 0;

	UFUNCTION()
	virtual void SetPitchAxisVal(float AxisValue) = 0;

	// Dash ability cooldown time
	UFUNCTION()
	virtual float GetDashAbilityCooldownTime() = 0;

	UFUNCTION()
	virtual void PassInteraction() = 0;
	
	// Just to play jump animation, a quick and dirty event
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category = "Jump")
	void JumpAnim();
};
