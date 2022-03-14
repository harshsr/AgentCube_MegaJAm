// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Neighbour.generated.h"

UCLASS()
class FURNITUREWARS_API ANeighbour : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANeighbour();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	//Components
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Components")
	UWidgetComponent* DialogWidget;

	// Player pawn ref (Bhampu)
	// TODO this is ok to do only because our player pawn never changes
	UPROPERTY(BlueprintReadOnly, Category = "Pawn Ref")
	APawn* PawnRef;

	// Player controller ref (BhampuController)
	UPROPERTY(BlueprintReadOnly, Category = "Pawn Ref")
	APlayerController* PlayerControllerRef;


private:
	// function to keep character aligned to ground slope and hooked to ground
	UFUNCTION()
	void FixAlignmentOnSlopes(float DeltaTime);

	// name of the root socket for distance fixup trace
	const FName RootSocketName = "RootSocket";

	// function to force DialogWidget to face camera
	UFUNCTION()
	void DialogFaceToCamera() const;

	
protected:
	// slope alignment interpolation speed
	UPROPERTY(EditAnywhere, Category = "SlopeAlignment")
	float SlopeInterpolateSpeed;


protected:
	// Custom perception system

	// bool for seeing
	UPROPERTY(BlueprintReadOnly, Category = "Custom Perception")
	bool bSeeingCustom;

	// Front sight radius
	UPROPERTY(EditDefaultsOnly, Category = "Custom Perception")
	float FrontSightRadius;
	
	// Near and backwards sight radius
	UPROPERTY(EditDefaultsOnly, Category = "Custom Perception")
	float BackSightRadius;
	
	// Front sight cone angle
	UPROPERTY(EditDefaultsOnly, Category = "Custom Perception")
	float FrontSightConeAngle;
	
	// function that performs perception check. Gets called every frame
	UFUNCTION()
	void CustomSight();

};
