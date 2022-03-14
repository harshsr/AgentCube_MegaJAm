// Copyright Epic Games, Inc. All Rights Reserved.


#include "FurnitureWarsGameModeBase.h"

#include "Public/Bhampu/Bhampu.h"
#include "Public/Bhampu/BhampuController.h"

AFurnitureWarsGameModeBase::AFurnitureWarsGameModeBase()
{

	DefaultPawnClass = ABhampu::StaticClass();
	PlayerControllerClass = ABhampuController::StaticClass();
}
