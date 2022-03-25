// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyActor.h"
#include "SpikyBallEnemyActor.generated.h"

/**
 * 
 */
UCLASS()
class CARTESTING_API ASpikyBallEnemyActor : public ABaseEnemyActor
{
	GENERATED_BODY()

public:
	ASpikyBallEnemyActor();

	virtual void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	FVector LocalUpVector{};

	void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	void SetUpVectorAsSplineUpAxis();
	void RotateSphereComponentToLocalUpVector();

	enum EBallState
	{
		Airborne,
		OnGround,
		Spiked
	};
	
	EBallState CurrentState{Airborne};
	bool bEnteringState{false};

	void ChangeState(EBallState NewState);

	FTimerHandle SpikeTimer;
	void TriggerSpikes();
};

inline void ASpikyBallEnemyActor::TriggerSpikes()
{
	ChangeState(EBallState::Spiked);
}
