// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsGrapplingComponent.h"

#include "CameraEffecttComponent.h"
#include "CarPawn.h"
#include "DrawDebugHelpers.h"
#include "GrappleSphereComponent.h"
#include "GravitySplineActor.h"
#include "Camera/CameraComponent.h"
#include "Chaos/GeometryParticlesfwd.h"
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
		StartLocationGrappleMesh = CarPawn->GrappleHookSphereComponent->GetRelativeLocation();
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

FRotator UPhysicsGrapplingComponent::GetTargetComponentRotator()
{
	return UKismetMathLibrary::MakeRotFromXZ(TargetComponentForwardVector, TargetComponentUpVector);
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

	//CarPawn->GrapplingHookMesh->OnComponentHit.AddDynamic(this, &UPhysicsGrapplingComponent::OnGrappleHit);
	
}

void UPhysicsGrapplingComponent::RetractGrapplingHook()
{
	//ResetTemporalVariables();
	EnterState(EGrappleStates::InActive);
}

void UPhysicsGrapplingComponent::ResetTemporalVariables()
{
	TempGrappleSphereComponent = nullptr;
	TargetComponentForwardVector = FVector::ZeroVector;
	TargetComponentUpVector = FVector::ZeroVector;
	bHoming = false;
	OnHookedDirection = FVector::ZeroVector;
	OnHookedVehicleTransfrom = FTransform::Identity;
	OnHookedSpeed = 0.f;
	HomingTargetLocation = FVector::ZeroVector;
	MoveToTargetModifier = 1.f;
	
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
	UE_LOG(LogTemp, Warning, TEXT("HIT!"))
	if (OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit GrappleSphereComponent %s"), *OtherComp->GetName())
		if (CurrentGrappleState == EGrappleStates::Traveling) // to ensure it dosen't gets called repeatedly
		{
			UGrappleSphereComponent* GrappleSphere = OtherActor->FindComponentByClass<UGrappleSphereComponent>(); // better?
			GrappleSphere->OnGrapple();
			TempGrappleSphereComponent = GrappleSphere;
			TargetComponentTransfrom = GrappleSphere->GetComponentTransform();
			TargetComponentForwardVector = GrappleSphere->GetForwardVector();
			TargetComponentUpVector = GrappleSphere->GetUpVector();
			
			
		}
	}
	EnterState(EGrappleStates::Hooked);
}


void UPhysicsGrapplingComponent::OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlap with -> %s"), *OtherActor->GetName())
	if (OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
	{
	UE_LOG(LogTemp, Warning, TEXT("(PhysicsGrappleComp) Sensor overlapped with %s"), *OtherActor->GetName())
		
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName())
		HomingTargetLocation = OtherComp->GetComponentLocation();
		bHoming = true;
	}
}

void UPhysicsGrapplingComponent::OnSensorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Stopped overlapping"));
		HomingTargetLocation = FVector::ZeroVector;
		bHoming = false;
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

		
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//CarPawn->GrapplingHookMesh->AddForce(CarPawn->GetActorForwardVector() * 10000000.f, FName(""), true);
		CarPawn->GrappleHookSphereComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CarPawn->GrappleHookSphereComponent->SetRelativeLocation(StartLocationGrappleMesh);
		ResetTemporalVariables();
		
	}
}

void UPhysicsGrapplingComponent::TravelingState()
{
	if (bEnterState)
	{
		bEnterState = false;
		bHoming = false;
		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(true);
		CarPawn->GrappleHookSphereComponent->AddImpulse(CarPawn->MainCamera->GetForwardVector() * FireGrappleSpeed, NAME_None, true);
	}
    
	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(), CarPawn->SphereComp->GetUpVector());
	CarPawn->GrappleSensor->SetWorldRotation(NewRot);

	if (!IsGrappleInsideOfRange())
	{
		EnterState(EGrappleStates::InActive);
	}
	
	if (bHoming)
	{
		//UE_LOG(LogTemp, Warning, TEXT("2"))
		FVector Vel = CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity();

		FVector Cross = FVector::CrossProduct(Vel, HomingTargetLocation - CarPawn->GrappleHookSphereComponent->GetComponentLocation());
		Cross = Cross.GetSafeNormal();
		//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Cross.X, Cross.Y, Cross.Z);
		if (Cross.SizeSquared() > 0.1f)
		{
			Vel = Vel.RotateAngleAxis(UGameplayStatics::GetWorldDeltaSeconds(this) * GrappleRotationSpeed, Cross);
			CarPawn->GrappleHookSphereComponent->SetPhysicsLinearVelocity(Vel);
		}
	}
}

void UPhysicsGrapplingComponent::HookedState()
{
	//UE_LOG(LogTemp, Warning, TEXT("HookedState"))
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
	MoveTowardsGrapple();
	FVector Direction = CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	TravelingDirection = Direction;
	
	//cheks if we are close enough (on exit)
	if ((CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation()).SizeSquared() < 400.f*400.f)
	{
		// handlig grappleSphere and its events
		if (TempGrappleSphereComponent != nullptr)
		{
			TempGrappleSphereComponent->OnReached();
			TempGrappleSphereComponent = nullptr;
		}
		
		EnterState(EGrappleStates::InActive);
	}
}

void UPhysicsGrapplingComponent::ReturningState()
{
	
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



