// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "CarTestingHud.generated.h"


UCLASS(config = Game)
class ACarTestingHud : public AHUD
{
	GENERATED_BODY()

public:
	ACarTestingHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
