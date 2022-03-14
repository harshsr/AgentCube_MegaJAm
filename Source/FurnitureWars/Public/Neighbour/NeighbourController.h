// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NeighbourController.generated.h"

/**
 * 
 */
UCLASS()
class FURNITUREWARS_API ANeighbourController : public AAIController
{
	GENERATED_BODY()

	public:

	// constructor
	ANeighbourController();

	// Tick
	virtual void Tick(float DeltaSeconds) override;
	
};
