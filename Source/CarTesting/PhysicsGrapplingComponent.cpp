// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsGrapplingComponent.h"

#include "CarPawn.h"
#include "DrawDebugHelpers.h"
#include "GravitySplineActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Grappling/GrappleTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPhysicsGrapplingComponent::UPhysicsGrapplingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPhysicsGrapplingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->IsA(ACarPawn::StaticClass()))
	{
		CarPawn = Cast<ACarPawn>(GetOwner());
		StartLocationGrappleMesh = CarPawn->GrappleHookMesh->GetRelativeLocation();
	}
	// ...
	
}


// Called every frame
void UPhysicsGrapplingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	switch (CurrentGrappleState)
	{
	case EGrappleStates::InActive:
		InActiveState();
		break;
	case EGrappleStates::Traveling:
		TravelingState();
		break;
	case EGrappleStates::Hooked:
		HookedState();
		break;
	case EGrappleStates::Returning:
		ReturningState();
		break;
	}

	//bEnterState = false;
	
}

void UPhysicsGrapplingComponent::FireGrapplingHook()
{
	
	EnterState(EGrappleStates::Traveling);

	//CarPawn->GrapplingHookMesh->OnComponentHit.AddDynamic(this, &UPhysicsGrapplingComponent::OnGrappleHit);
	
}

void UPhysicsGrapplingComponent::RetractGrapplingHook()
{
	EnterState(EGrappleStates::InActive);
}

void UPhysicsGrapplingComponent::OnGrappleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit"))
	if (OtherActor->IsA(AGrappleTarget::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("did set visibility"))
		Cast<AGrappleTarget>(OtherActor)->SetVisbility(false);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Grapple hit simethung"))
	EnterState(EGrappleStates::Hooked);
}

void UPhysicsGrapplingComponent::OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap with -> %s"), *OtherActor->GetName())
	if (OtherActor->IsA(AGrappleTarget::StaticClass()))
	{
		
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName())
		HomingTargetLocation = OtherActor->GetActorLocation();
		bHoming = true;
	}
}

void UPhysicsGrapplingComponent::EnterState(EGrappleStates NewState)
{
	bEnterState = true;
	CurrentGrappleState = NewState;
}

void UPhysicsGrapplingComponent::InActiveState()
{
	if (bEnterState){
		bEnterState = false;

		
		CarPawn->GrappleHookMesh->SetSimulatePhysics(false);
		CarPawn->GrappleHookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//CarPawn->GrapplingHookMesh->AddForce(CarPawn->GetActorForwardVector() * 10000000.f, FName(""), true);
		CarPawn->GrappleHookMesh->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CarPawn->GrappleHookMesh->SetRelativeLocation(StartLocationGrappleMesh);

		HomingTargetLocation = FVector::ZeroVector;
		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		

		
	}
	
	
}

void UPhysicsGrapplingComponent::TravelingState()
{
	if (bEnterState)
	{
		bEnterState = false;
		bHoming = false;
		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CarPawn->GrappleHookMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		CarPawn->GrappleHookMesh->SetSimulatePhysics(true);
		CarPawn->GrappleHookMesh->AddImpulse(CarPawn->MainCamera->GetForwardVector() * FireGrappleSpeed, NAME_None, true);
	}
    
	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(CarPawn->GrappleHookMesh->GetPhysicsLinearVelocity(), CarPawn->SphereComp->GetUpVector());
	CarPawn->GrappleSensor->SetWorldRotation(NewRot);
	//CarPawn->GrappleHookMesh->AddForce(CarPawn->GrappleHookMesh->GetPhysicsLinearVelocity().GetSafeNormal() * 10.f, NAME_None, true);
	
	//changing the velocity if GrappleTarget inRange
	//the location is updated in the OnSensorOverlap
	
	if (bHoming)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Not zero vector"))
		FVector Vel = CarPawn->GrappleHookMesh->GetPhysicsLinearVelocity();

		FVector Cross = FVector::CrossProduct(Vel, HomingTargetLocation - CarPawn->GrappleHookMesh->GetComponentLocation());
		Cross = Cross.GetSafeNormal();
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Cross.X, Cross.Y, Cross.Z);
		if (Cross.SizeSquared() > 0.1f)
		{
			Vel = Vel.RotateAngleAxis(UGameplayStatics::GetWorldDeltaSeconds(this) * GrappleRotationSpeed, Cross);
			CarPawn->GrappleHookMesh->SetPhysicsLinearVelocity(Vel);
			
		}
	}


	
	
}

void UPhysicsGrapplingComponent::HookedState()
{
	//UE_LOG(LogTemp, Warning, TEXT("HookedState"))
	if (bEnterState)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Disables physics"))
		CarPawn->GrappleHookMesh->SetSimulatePhysics(false);
		CarPawn->GrappleHookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OnHookedVelocity = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size();
		OnHookedDirection = (CarPawn->GrappleHookMesh->GetComponentLocation() - CarPawn->GetActorLocation()).GetSafeNormal();
		
		MoveToTargetModifier = 1.f;
		
		bEnterState = false;
	}
	MoveTowardsGrapple();
	FVector Direction = CarPawn->GrappleHookMesh->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	TravelingDirection = Direction;
	
	//cheks if we are close enough
	if ((CarPawn->GrappleHookMesh->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation()).SizeSquared() < 200.f*200.f)
	{
		
		
		EnterState(EGrappleStates::InActive);
	}
}

void UPhysicsGrapplingComponent::ReturningState()
{
	
}

void UPhysicsGrapplingComponent::MoveTowardsGrapple()
{
	MoveToTargetModifier += UGameplayStatics::GetWorldDeltaSeconds(this) * MoveToTargetAcceleration;
	FVector Direction = CarPawn->GrappleHookMesh->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	CarPawn->AddActorWorldOffset(Direction * GetOnHookedVelocitySize() * MoveToTargetModifier * UGameplayStatics::GetWorldDeltaSeconds(this), true);
}

/**
 * @brief Checks is the car should be in grapple state
 * @return true if we should be in grapple state
 */
bool UPhysicsGrapplingComponent::ValidGrappleState()
{
	if (GetCurrentGrappleState() == EGrappleStates::Hooked && bEnterState == false)
	{
		return true;
	}
	return false;
}



