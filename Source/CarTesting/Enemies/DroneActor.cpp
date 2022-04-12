// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneActor.h"

#include "CarTesting/CarPawn.h"
#include "CarTesting/GrappleSphereComponent.h"
#include "CarTesting/GravitySplineActor.h"
#include "Components/SphereComponent.h"

ADroneActor::ADroneActor()
{
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());
	GrappleSphereComponent->SetIsEnabled(true);
	GrappleSphereComponent->SetIsEatable(true);
}

void ADroneActor::BeginPlay()
{
	Super::BeginPlay();
	GrappleSphereComponent->OnGrappleHitEvent.AddDynamic(this, &ADroneActor::Grappled);
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ADroneActor::Reached);
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
	TargetLocation = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector()*ForwardOffset;
	if (bEnteringState)
	{
		InterceptSpeed = (TargetLocation-GetActorLocation()).Size()/InterceptTime;
		bEnteringState = false;
	}
	TargetLocation.Z = GetActorLocation().Z;
	SetActorRotation((TargetLocation - GetActorLocation()).Rotation());
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

	FRotator CurrentRot{GetActorRotation()};
	FRotator TargetRot{(PlayerPawn->GetActorLocation()-GetActorLocation()).Rotation()};
	FRotator NewRot{FMath::RInterpTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), 5.f)};
	SetActorRotation(NewRot);
}

void ADroneActor::Grappled(FTransform SphereCompTransform)
{
	//TODO: Add behaviour for being grappled.
}

void ADroneActor::Reached(float AddSpeedAmount)
{
	//TODO: Add animation for destruction.
	Destroy();
}

void ADroneActor::Move(FVector Target)
{
	InterceptTimer += GetWorld()->GetDeltaSeconds();
	SphereComp->AddRelativeLocation((TargetLocation-GetActorLocation()).GetSafeNormal()*InterceptSpeed*GetWorld()->GetDeltaSeconds());
	if (InterceptTimer >= InterceptTime)
	{
		ChangeState(EDroneState::Attacking);
	}
}
