// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChaosSystemPawn.h"
#include "ChaosSystemSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class AChaosSystemSportsCar : public AChaosSystemPawn
{
	GENERATED_BODY()
	
public:

	AChaosSystemSportsCar();
};
