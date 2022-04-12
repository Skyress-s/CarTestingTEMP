// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyActor.h"
#include "DroneActor.generated.h"

/**
 * 
 */
UCLASS()
class CARTESTING_API ADroneActor : public ABaseEnemyActor
{
	GENERATED_BODY()

public:
	ADroneActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		float ForwardOffset{500.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		float InterceptTime{3.f};
	float InterceptTimer{};
	float InterceptSpeed{};
	
protected:
	FVector TargetLocation{};

	enum EDroneState
	{
		Idle,
		Intercepting,
		Attacking
	};

	EDroneState CurrentState{EDroneState::Idle};
	bool bEnteringState{false};

	void ChangeState(EDroneState NewState)
	{
		bEnteringState = true;
		CurrentState = NewState;
	}

	void InterceptingState();
	void AttackingState();

	void Move(FVector Target);
};
