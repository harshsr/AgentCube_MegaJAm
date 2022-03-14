// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionSys.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionSys : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FURNITUREWARS_API IInteractionSys
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Every actor that needs to interact to Bhampu implements this interface
	// And uses the following function to define its interaction behaviour
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category = "InteractionSystem")
	void Interact();
};
