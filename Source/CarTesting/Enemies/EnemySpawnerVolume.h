// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyActor.h"
#include "Engine/TriggerVolume.h"
#include "EnemySpawnerVolume.generated.h"

/**
 * 
 */
UCLASS()
class CARTESTING_API AEnemySpawnerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	AEnemySpawnerVolume();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		TSubclassOf<ABaseEnemyActor> EnemyClassToSpawn;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		int32 NumberOfEnemiesToSpawn {1};
};
