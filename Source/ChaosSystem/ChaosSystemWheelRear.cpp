// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaosSystemWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UChaosSystemWheelRear::UChaosSystemWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}