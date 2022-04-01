// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarTesting/CarTestingGameMode.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class CARTESTING_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//my deseg
private:
	//components
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UArrowComponent* BaseArrow{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UStaticMeshComponent* MainMesh{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UBoxComponent* BoxComponent{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UArrowComponent* SpawnArrow{};

	//variables
	UPROPERTY()
		class ACarTestingGameMode* GameModee{};
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Checkpoint")
		class AGravitySplineActor* GravitySplineActor{};

	//funcs
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// public funcs
	UFUNCTION()
		UArrowComponent* GetSpawnArrow() const {return SpawnArrow; }
	UFUNCTION()
		AGravitySplineActor* GetCheckpointGravitySpline() const {return GravitySplineActor; }

};
