// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsGrapplingComponent.h"

#include "BoostComponent.h"
#include "CameraEffecttComponent.h"
#include "CarPawn.h"
#include "DrawDebugHelpers.h"
#include "GrappleSphereComponent.h"
#include "GravitySplineActor.h"
#include "NeckComponent.h"
#include "Camera/CameraComponent.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
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
		StartLocationGrappleMesh = CarPawn->GrappleHookSphereComponent->GetRelativeLocation();
	}
	// ...
	
}


// Called every frame
void UPhysicsGrapplingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
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
	case EGrappleStates::HookedEatable:
		HookedEatableState();
		break;
	case EGrappleStates::Returning:
		ReturningState();
		break;
	}

	//bEnterState = false;
	
}

bool UPhysicsGrapplingComponent::IsGrappleInsideOfRange()
{
	float SqrDistance = (CarPawn->SphereComp->GetComponentLocation() - CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	//using squared distance since it cheaper
	if (SqrDistance < MaxGrappleDistance * MaxGrappleDistance)
		return true;

	return false;
}

void UPhysicsGrapplingComponent::FireGrapplingHook()
{
	
	EnterState(EGrappleStates::Traveling);
	
}

void UPhysicsGrapplingComponent::RetractGrapplingHook()
{
	//ResetTemporalVariables();
	EnterState(EGrappleStates::Returning);
}

void UPhysicsGrapplingComponent::ResetTemporalVariables()
{
	
	OnHookedDirection = FVector::ZeroVector;
	OnHookedVehicleTransfrom = FTransform::Identity;
	OnHookedSpeed = 0.f;
	TargetGrappableComponent = nullptr;
	MoveToTargetModifier = 1.f;
	CurrentReturnTime = 0.f;
	
}

/**
 * @brief When the grappleComponent hits a grappleHit channel
 * @param OverlappedComponent 
 * @param OtherActor 
 * @param OtherComp 
 * @param OtherBodyIndex 
 * @param bFromSweep 
 * @param SweepResult 
 */
void UPhysicsGrapplingComponent::OnGrappleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CurrentGrappleState != EGrappleStates::Traveling)
		return;
	
	if (OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
	{
		
		if (CurrentGrappleState == EGrappleStates::Traveling) // to ensure it dosen't gets called repeatedly
		{
			UGrappleSphereComponent* GrappleSphereComponent = OtherActor->FindComponentByClass<UGrappleSphereComponent>(); // better? Better

			if (GrappleSphereComponent->IsEnabled())
			{
				GrappleSphereComponent->OnGrapple();
				TargetGrappableComponent = GrappleSphereComponent;

				//eatable v grappable logic
				if (GrappleSphereComponent->IsEatable())
					EnterState(EGrappleStates::HookedEatable);
				else
					EnterState(EGrappleStates::Hooked);
			}
			
		}
	}
}


void UPhysicsGrapplingComponent::OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// guard clusase -> if we are not in travveling OR if we alleready are homing, dont change values
	if (CurrentGrappleState != EGrappleStates::Traveling || TargetGrappableComponent != nullptr)
		return;
	
	
	if (OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
	{
		UGrappleSphereComponent* GrappleSphereComponent = OtherActor->FindComponentByClass<UGrappleSphereComponent>();
		if (GrappleSphereComponent->IsEnabled()) // is it enabled
		{
			TargetGrappableComponent = GrappleSphereComponent;
		}
	}
}

void UPhysicsGrapplingComponent::OnSensorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//does nothing currently
	//TODO if no logic is needed, remove this func
	if (CurrentGrappleState != EGrappleStates::Traveling)
	{
		return;
	}
	
}

void UPhysicsGrapplingComponent::EnterState(EGrappleStates NewState)
{
	bEnterState = true;
	CurrentGrappleState = NewState;
}

void UPhysicsGrapplingComponent::InActiveState()
{
	UE_LOG(LogTemp, Warning, TEXT("InaciveState"))
	if (bEnterState){
		bEnterState = false;
		
		//CarPawn->GrapplingHookMesh->AddForce(CarPawn->GetActorForwardVector() * 10000000.f, FName(""), true);
		CarPawn->GrappleHookSphereComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CarPawn->GrappleHookSphereComponent->SetRelativeLocation(StartLocationGrappleMesh);
		ResetTemporalVariables();
		
	}
	CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh();
}

void UPhysicsGrapplingComponent::TravelingState()
{
	UE_LOG(LogTemp, Warning, TEXT("Traveling state"))
	if (bEnterState)
	{
		bEnterState = false;
		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(true);
		CarPawn->GrappleHookSphereComponent->AddImpulse(CarPawn->MainCamera->GetForwardVector() * FireGrappleSpeed, NAME_None, true);
	}
    
	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(), CarPawn->SphereComp->GetUpVector());
	CarPawn->GrappleSensor->SetWorldRotation(NewRot);
	
	CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh();
	
	if (!IsGrappleInsideOfRange())
	{
		EnterState(EGrappleStates::Returning);
	}
	
	if (TargetGrappableComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Homing"))
		FVector Vel = CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity();
		FVector ToTarget = TargetGrappableComponent->GetComponentLocation() - CarPawn->GrappleHookSphereComponent->GetComponentLocation();
		FVector Cross = FVector::CrossProduct(Vel, ToTarget);
		Cross = Cross.GetSafeNormal();
		float Angle = ACarPawn::UnsignedAngle(Vel, ToTarget);
		if (Angle > 5.f)
		{
			Vel = Vel.RotateAngleAxis(UGameplayStatics::GetWorldDeltaSeconds(this) * GrappleRotationSpeed, Cross);
			CarPawn->GrappleHookSphereComponent->SetPhysicsLinearVelocity(Vel);
			
		}
		
	}
}

void UPhysicsGrapplingComponent::HookedState()
{
	UE_LOG(LogTemp, Warning, TEXT("HookedState"))
	
	if (bEnterState)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Disables physics"))
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		OnHookedSpeed = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size();
		if (OnHookedSpeed >= HighestOnHookedSpeed) // clamps the velocity
			OnHookedSpeed = HighestOnHookedSpeed;
		else if (OnHookedSpeed <= LowestOnHookedSpeed)
			OnHookedSpeed = LowestOnHookedSpeed;
		
		
		OnHookedDirection = (CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation()).GetSafeNormal();
		OnHookedVehicleTransfrom = CarPawn->GetTransform();
		CarPawn->CameraEffectComponent->PlayCameraEffect();
		
		
		bEnterState = false;
	}

	
	CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh();
	MoveTowardsGrapple();
	FVector Direction = CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	TravelingDirection = Direction;

	//updates grapplehookcomponent location
	if (TargetGrappableComponent)
	{
		CarPawn->GrappleHookSphereComponent->SetWorldLocation(TargetGrappableComponent->GetComponentLocation());
	}
	
	
	//cheks if we are close enough (on exit)
	if ((CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation()).SizeSquared() < 400.f*400.f)
	{
		// handlig grappleSphere and its events
		if (TargetGrappableComponent != nullptr)
		{
			TargetGrappableComponent->OnReached();

			if (TargetGrappableComponent->IsOverrideReleaseVelocity())
			{
				OnHookedDirection = TargetGrappableComponent->GetForwardVector();
			}
			OnHookedSpeed += TargetGrappableComponent->GetAddSpeed();
		}
		
		EnterState(EGrappleStates::Returning);
	}
}

void UPhysicsGrapplingComponent::HookedEatableState()
{
	UE_LOG(LogTemp, Warning, TEXT("HookedEatableState"))
	if (bEnterState)
	{
		bEnterState = false;
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	CurrentReturnTime += GetWorld()->GetDeltaSeconds();
	float lerp = CurrentReturnTime / ReturnEatableTime; // a lerp from 0 -> 1
	float invertedLerp = 1.f - lerp;
	float length = CarPawn->NeckSpline->GetSplineLength();

	length = length * invertedLerp;
	
	FVector NewPos = CarPawn->NeckSpline->GetLocationAtDistanceAlongSpline(length, ESplineCoordinateSpace::World);
	CarPawn->GrappleHookSphereComponent->SetWorldLocation(NewPos);

	
	//CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh(0.f, length);

	// sets the eatable components location and rotation
	TargetGrappableComponent->GetOwner()->SetActorLocation(CarPawn->GrappleHookSphereComponent->GetComponentLocation());
	TargetGrappableComponent->GetOwner()->SetActorRotation(CarPawn->GrappleHookSphereComponent->GetComponentRotation());

	//are we close to home
	float distanceSqr = (CarPawn->SphereComp->GetComponentLocation() -
		CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	if (distanceSqr < 1000.f* 1000.f) // when its at us!
	{
		CarPawn->HandleBoost();
		TargetGrappableComponent->OnReached();
		
		
		EnterState(EGrappleStates::InActive);
	}
	
}

void UPhysicsGrapplingComponent::ReturningState()
{
	UE_LOG(LogTemp, Warning, TEXT("returning State"))
	if (bEnterState){
		bEnterState = false;
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	CurrentReturnTime += GetWorld()->GetDeltaSeconds();
	float lerp = CurrentReturnTime / ReturnTime; // a lerp from 0 -> 1
	float invertedLerp = 1.f - lerp;
	float length = CarPawn->NeckSpline->GetSplineLength();

	length = length * invertedLerp;
	
	FVector NewPos = CarPawn->NeckSpline->GetLocationAtDistanceAlongSpline(length, ESplineCoordinateSpace::World);
	CarPawn->GrappleHookSphereComponent->SetWorldLocation(NewPos);

	
	//CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh(0.f, length);

	//are we close to home
	float distanceSqr = (CarPawn->SphereComp->GetComponentLocation() -
		CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	if (distanceSqr < 1000.f* 1000.f)
	{
		EnterState(EGrappleStates::InActive);
		
	}
}

void UPhysicsGrapplingComponent::MoveTowardsGrapple()
{
	MoveToTargetModifier += UGameplayStatics::GetWorldDeltaSeconds(this) * MoveToTargetAcceleration;
	/*if (MoveToTargetModifier > 4.f)
	{
		MoveToTargetModifier = 4.f;
	}*/
	
	FVector Direction = CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	CarPawn->AddActorWorldOffset(Direction * GetOnHookedVelocitySize() * MoveToTargetModifier * UGameplayStatics::GetWorldDeltaSeconds(this), false);
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



