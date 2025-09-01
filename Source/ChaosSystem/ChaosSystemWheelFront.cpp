// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaosSystemWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UChaosSystemWheelFront::UChaosSystemWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}