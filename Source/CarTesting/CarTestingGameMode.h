// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "CarTestingGameMode.generated.h"

UCLASS(minimalapi)
class ACarTestingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACarTestingGameMode();

	// my deseg ---------
private:
	//this is for if we need to use it!
	// UPROPERTY()
	// 	TArray<ACheckpoint*> CheckpointOrder{};

	//checkpoints tells the gamemode what was the last checkpoint the player touched
	UPROPERTY()
		class ACheckpoint* LastCheckpoint{};
public:
	UFUNCTION()
		class ACheckpoint* GetLastCheckpoint() const {return LastCheckpoint; }
	UFUNCTION()
		void SetLastCheckpoint(class ACheckpoint* NewLastCheckpoint) {LastCheckpoint = NewLastCheckpoint; }
	
};



