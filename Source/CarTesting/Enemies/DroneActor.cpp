// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneActor.h"

#include "CarTesting/CarPawn.h"
#include "CarTesting/GravitySplineActor.h"
#include "Components/SphereComponent.h"

ADroneActor::ADroneActor()
{
}

void ADroneActor::BeginPlay()
{
	Super::BeginPlay();
}

void ADroneActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (CurrentState)
	{
	case EDroneState::Intercepting:
		InterceptingState();
		break;
	case EDroneState::Attacking:
		AttackingState();
		break;
	case EDroneState::Idle:
		if (PlayerPawn && GravitySplineActive) ChangeState(Intercepting);
		break;
	}
	if (!PlayerPawn)
	{
		PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
	else if (!GravitySplineActive)
    {
    	GravitySplineActive = PlayerPawn->GravitySplineActive;
    }
}

void ADroneActor::InterceptingState()
{
	if (bEnteringState)
	{
		InterceptSpeed = (TargetLocation-GetActorLocation()).Size()/InterceptTime;
		TargetLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector()*ForwardOffset;
		bEnteringState = false;
		
	}
	TargetLocation.Z = GetActorLocation().Z;
	Move(TargetLocation);
	if (GetActorLocation().Equals(TargetLocation))
	{
		ChangeState(EDroneState::Attacking);
	}
}

void ADroneActor::AttackingState()
{
	if (bEnteringState)
	{
		bEnteringState = false;
	}
}

void ADroneActor::Move(FVector Target)
{
	InterceptTimer += GetWorld()->GetDeltaSeconds();
	SphereComp->AddRelativeLocation((TargetLocation-GetActorLocation()).GetSafeNormal()*InterceptSpeed*InterceptTimer);
	if (InterceptTimer >= InterceptTime)
	{
		ChangeState(EDroneState::Attacking);
	}
}
