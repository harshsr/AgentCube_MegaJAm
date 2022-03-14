// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FurnitureWars/Public/Interfaces/BhampuCppInterface.h"
#include "GameFramework/PlayerController.h"
#include "BhampuController.generated.h"

/**
 * 
 */
UCLASS()
class FURNITUREWARS_API ABhampuController : public APlayerController
{
	GENERATED_BODY()

	public:
	// Constructor
	ABhampuController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

protected:

	IBhampuCppInterface* BhampuTagsInterface = nullptr;

	UPROPERTY(BlueprintReadWrite)
	bool bStealthOverlap;
	
	UPROPERTY(BlueprintReadWrite)
	bool bStealthWarping;
	
	UPROPERTY(BlueprintReadOnly)
	bool bInteractionAvailable;

	UFUNCTION(BlueprintImplementableEvent)
	void PromptInteractionUI(bool bInteraction);


private:
	UPROPERTY()
	bool bDashed;
	
	UPROPERTY()
	float DashAbilityCooldownTime;

	
	protected:
	// UI dash cooldown update event
	UFUNCTION(BlueprintImplementableEvent)
	void UIDashUpdate(float AbilityCooldownFraction);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ReadyToHide();
	
	UFUNCTION(BlueprintNativeEvent)
	void UnHide();


protected:
	// Input bind functions
	UFUNCTION()
	void ForwardAxis(float AxisValue);

	UFUNCTION()
	void RightAxis(float AxisValue);

	UFUNCTION()
	void YawAxis(float AxisValue);

	UFUNCTION()
	void PitchAxis(float AxisValue);

	UFUNCTION()
	void JumpAction();

	UFUNCTION()
	void DashAction();

	UFUNCTION()
	void HideAction();

	UFUNCTION()
	void InteractionAction();
	
};
