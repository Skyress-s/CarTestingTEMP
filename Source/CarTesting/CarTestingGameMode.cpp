// Copyright Epic Games, Inc. All Rights Reserved.

#include "CarTestingGameMode.h"
#include "CarTestingPawn.h"
#include "CarTestingHud.h"

ACarTestingGameMode::ACarTestingGameMode()
{
	DefaultPawnClass = ACarTestingPawn::StaticClass();
	HUDClass = ACarTestingHud::StaticClass();
}
