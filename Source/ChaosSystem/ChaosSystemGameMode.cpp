// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaosSystemGameMode.h"
#include "ChaosSystemPlayerController.h"

AChaosSystemGameMode::AChaosSystemGameMode()
{
	PlayerControllerClass = AChaosSystemPlayerController::StaticClass();
}
