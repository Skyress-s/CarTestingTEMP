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
	SphereComp->AddForce(-LocalUpVector*Acceleration, NAME_None, true);
	switch (CurrentState)
	{
	case EBallState::Airborne:
		AirborneState();
		break;
	case EBallState::OnGround:
		GroundedState();
		break;
	case EBallState::Spiked:
		SpikedState();
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

void ASpikyBallEnemyActor::AirborneState()
{
	if (bEnteringState) bEnteringState = false;
	if (IsGrounded()) ChangeState(EBallState::OnGround);
	UE_LOG(LogTemp, Warning, TEXT("Airborne"));
}

void ASpikyBallEnemyActor::GroundedState()
{
	if (bEnteringState)
	{
		// if (SphereComp->IsSimulatingPhysics())
		// {
		// 	SphereComp->SetSimulatePhysics(false);
		// 	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// }
		GetWorld()->GetTimerManager().SetTimer(SpikeTimerHandle, this, &ASpikyBallEnemyActor::TriggerSpikes, SpikeTimer);
		bEnteringState = false;
	}
	LookAtPlayer();
	UE_LOG(LogTemp, Warning, TEXT("Grounded"));
}

void ASpikyBallEnemyActor::SpikedState()
{
	if (bEnteringState)
	{
		bEnteringState = false;
	}
	Move();
	UE_LOG(LogTemp, Warning, TEXT("Spiked"));
}

void ASpikyBallEnemyActor::LookAtPlayer()
{
	FRotator CurrentRot{GetActorRotation()};
	FVector NewDir{(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal()};
	FRotator NewRot = FMath::RInterpTo(CurrentRot, NewDir.Rotation(), GetWorld()->GetDeltaSeconds(),0.75f);
	
	SetActorRotation(NewRot, ETeleportType::TeleportPhysics);
}

void ASpikyBallEnemyActor::Move()
{
	FVector MoveDir{(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal()};
	// MoveDir.Z = 0.f;
	if (SphereComp->GetPhysicsLinearVelocity().Size() <= MaxSpeed)
	{
		SphereComp->AddForce(MoveDir*Acceleration, NAME_None, true);
	}
}
