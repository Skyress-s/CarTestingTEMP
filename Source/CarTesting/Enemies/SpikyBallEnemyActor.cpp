// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikyBallEnemyActor.h"

#include "CarTesting/GravitySplineActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASpikyBallEnemyActor::ASpikyBallEnemyActor() : ABaseEnemyActor()
{
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetSphereRadius(200.f);
}

void ASpikyBallEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	LocalUpVector = GetActorUpVector();
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASpikyBallEnemyActor::OnBeginOverLap);
	AGravitySplineActor* ClosestGravitySpline{};
	ClosestGravitySpline = GetClosestGravitySpline();
	if (ClosestGravitySpline)
	{
		GravitySplineActive = ClosestGravitySpline;
	}
	SetUpVectorAsSplineUpAxis();
	RotateSphereComponentToLocalUpVector();
}

void ASpikyBallEnemyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SphereComp->AddForce(-LocalUpVector*981.f, NAME_None, true);

	switch (CurrentState)
	{
	case EBallState::Airborne:
		break;
	case EBallState::OnGround:
		break;
	case EBallState::Spiked:
		break;
	}
}

void ASpikyBallEnemyActor::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverLap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
}

void ASpikyBallEnemyActor::SetUpVectorAsSplineUpAxis()
{
	if (GravitySplineActive)
	{
		LocalUpVector = GravitySplineActive->GetAdjustedUpVectorFromLocation(SphereComp->GetComponentLocation());
	}
}

void ASpikyBallEnemyActor::RotateSphereComponentToLocalUpVector()
{
	FRotator NewSphereRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
	SphereComp->SetWorldRotation(NewSphereRot);
}

void ASpikyBallEnemyActor::ChangeState(EBallState NewState)
{
	bEnteringState = true;
	CurrentState = NewState;
}
