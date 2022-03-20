// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPawn.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Camera/CameraComponent.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "BoostComponent.h"
#include "CameraEffecttComponent.h"
#include "GravitySplineActor.h"
#include "HighGravityZone.h"
#include "PhysicsGrapplingComponent.h"
#include "Chaos/KinematicTargets.h"
#include "CollisionAnalyzer/Public/ICollisionAnalyzer.h"
#include "Components/SplineComponent.h"
#include "Grappling/GrappleComponent.h"
#include "Grappling/GrappleTarget.h"


// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SphereComp->BodyInstance.bLockRotation = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Car Mesh"));
	CarMesh->SetupAttachment(GetRootComponent());
	CarMesh->SetSimulatePhysics(false);
	//SetRootComponent(CarMesh);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20.f;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainCamera->SetupAttachment(CameraBoom);

	ArrowRayCastStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow RayCastStart"));
	ArrowRayCastStart->SetupAttachment(GetRootComponent());
	
	BoostComponent = CreateDefaultSubobject<UBoostComponent>(TEXT("Boost Component"));
	
	//GrappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComponent"));
	
	
	GrappleHookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrapplingHookMesh"));
	GrappleHookMesh->SetupAttachment(SphereComp);
	GrappleHookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrappleHookMesh->SetEnableGravity(false);

	GrappleSensor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrappleSensor"));
	GrappleSensor->SetupAttachment(GrappleHookMesh);
	GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//response to channel manually set in BP

    PhysicsGrappleComponent = CreateDefaultSubobject<UPhysicsGrapplingComponent>(TEXT("PhysicsGrappleComponent"));

	CameraEffectComponent = CreateDefaultSubobject<UCameraEffecttComponent>(TEXT("CameraEffectComponent"));
	
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();
	// Hit and phyus
	SphereComp->OnComponentHit.AddDynamic(this, &ACarPawn::OnHitt);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnBeginOverLap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ACarPawn::OnEndOverLap);

	GrappleHookMesh->OnComponentHit.AddDynamic(PhysicsGrappleComponent, &UPhysicsGrapplingComponent::OnGrappleHit);
	GrappleSensor->OnComponentBeginOverlap.AddDynamic(PhysicsGrappleComponent, &UPhysicsGrapplingComponent::OnSensorOverlap);
	GrappleSensor->OnComponentEndOverlap.AddDynamic(PhysicsGrappleComponent, &UPhysicsGrapplingComponent::OnSensorEndOverlap);
	CameraEffectComponent->SetCameraCurrent(MainCamera);

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents, false /*or true*/);
	//UE_LOG(LogTemp, Warning, TEXT("%d"), PrimitiveComponents.Num())
		BoostComponent->PhysComp = PrimitiveComponents[0];
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *PrimitiveComponents[0]->GetName())
	StartPlayerLocation = SphereComp->GetComponentLocation();
	
	//setting camera lag
	OnStartCameraLag = FVector2D(CameraBoom->CameraLagSpeed, CameraBoom->CameraRotationLagSpeed);
	StartCameraBoomLength = CameraBoom->TargetArmLength;
}

void ACarPawn::RotateSphereCompToLocalUpVector()
{
	//rotates sphere
	FRotator NewSphereRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
	SphereComp->SetWorldRotation(NewSphereRot);
}

void ACarPawn::ApplyGravity()
{
	//gravity
	if (GravitySplineActive != nullptr)
	{
		FVector GravityUpVector = GravitySplineActive->GetAdjustedUpVectorFromLocation(
			SphereComp->GetComponentLocation());
		GravityUpVector.Normalize();

		if (SphereComp->IsSimulatingPhysics())
		{
			// UE_LOG(LogTemp, Warning, TEXT("Is simulating physics"))
			SphereComp->AddForce(-GravityUpVector * 68.1f * 40.f * GravityMod, FName(), true);
		}
	}
}

void ACarPawn::TiltCarMesh(FVector AsymVector)
{
	//orients the mesh
	FRotator NewRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector + AsymVector * 0.0001f,
	                                                    GetActorForwardVector());
	/*
	float sign = NewRot.Roll / abs(NewRot.Roll);
	if (abs(NewRot.Roll) > 45.f)
	{
		NewRot.Roll = 45.f * sign;
	}*/
	
	
	CarMesh->SetWorldRotation( FMath::RInterpTo(CarMesh->GetComponentRotation(),
	                                            NewRot,
	                                            UGameplayStatics::GetWorldDeltaSeconds(this),
	                                            5.f
	) );

	//clamps the roll rotation
	float ClampValue = 45.f;
	FRotator LocalRot = CarMesh->GetRelativeRotation();
	if (LocalRot.Roll > ClampValue)
	{
		LocalRot.Roll = ClampValue;
	}
	else if (LocalRot.Roll < -ClampValue)
	{
		LocalRot.Roll = -ClampValue;
	}
	CarMesh->SetRelativeRotation(LocalRot);
	
}

void ACarPawn::HandleAsymFriction(FVector& AsymVector)
{
	AsymVector = CalcAsymVector();
	if (SphereComp->IsSimulatingPhysics())
	{
		SphereComp->AddForce(AsymVector* 30.f);
	}
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// state machine
	switch (CurrentVehicleState)
	{
	case EVehicleState::Driving:
		StateDriving();
		break;
	case EVehicleState::AirBorne:
		StateAirBorne();
		break;
	case EVehicleState::Dashing:
		StateDashing();
		break;
	case EVehicleState::Grappling:
		StateGrappling();
		break;
		
	}

	//sets bEnterstate to false so it wont run until we enter a new state
	//bEnterState = false;
	
	//DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), SphereComp->GetComponentLocation() + SphereComp->GetUpVector() * 300.f, FColor::Green, false, 0.5f);
	//DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), SphereComp->GetComponentLocation() + SphereComp->GetRightVector() * 300.f, FColor::Green, false, 0.5f);
	

}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &ACarPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACarPawn::MoveYAxis);
	PlayerInputComponent->BindAxis("LookRight", this, &ACarPawn::LookXAxis);
	PlayerInputComponent->BindAxis("LookUp", this, &ACarPawn::LookYAxis);
	
	// Action binding
	FInputActionBinding& action = PlayerInputComponent->BindAction("Boost", EInputEvent::IE_Pressed, this, &ACarPawn::HandleBoost);
	//action.bConsumeInput = false;
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACarPawn::ToggleGrappleHook);

	
}

void ACarPawn::EnterState(EVehicleState NewState)
{
	bEnterState = true;
	CurrentVehicleState = NewState;
}

void ACarPawn::StateDriving()
{
	if (bEnterState)
	{
		bEnterState = false;
	}
	ApplyGravity();
	SetUpVectorAsSplineUpAxis();
	//SpeedHandleCameraBoomEffect(true);
	if (IsGrounded())
	{
		
		
		FVector AsymVector;
		HandleAsymFriction(AsymVector);
		TiltCarMesh(AsymVector);
	
		// rotates sphere comp
		RotateSphereCompToLocalUpVector();
		
	}
	else
	{
		EnterState(EVehicleState::AirBorne);
	}


	//should we be grappling
	if (PhysicsGrappleComponent->ValidGrappleState())
	{
		EnterState(EVehicleState::Grappling);
	}
}

void ACarPawn::StateGrappling()
{
	if (bEnterState)
	{
		bEnterState = false;
		SphereComp->SetSimulatePhysics(false);

		
		CameraBoom->CameraLagSpeed = GrapplingCameraLag.X;
		CameraBoom->CameraRotationLagSpeed = GrapplingCameraLag.Y;
	}

	//SpeedHandleCameraBoomEffect(false);
	
	//orients the sphere comp
	/*FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(PhysicsGrappleComponent->GetTravelingDirection(), GravitySplineActive->GetAdjustedUpVectorFromLocation(SphereComp->GetComponentLocation()));
	SphereComp->SetWorldRotation(NewRot);
	CameraBoom->SetRelativeRotation(FRotator(-24.f, 0.f, 0.f));*/
	float StartDistance = (PhysicsGrappleComponent->GetOnHookedVehicleTransform().GetLocation() -  GrappleHookMesh->GetComponentLocation()).Size();
	float CurrentDistance = (SphereComp->GetComponentLocation() -  GrappleHookMesh->GetComponentLocation()).Size();
	float lerpFactor = CurrentDistance / StartDistance; // at start will be 1, and will progress towards 0
	lerpFactor = 1.f -lerpFactor; // at start will be 0, will progress towards 1

	/*FRotator NewRot = FMath::Lerp<FRotator, float>(
		PhysicsGrappleComponent->GetOnHookedVehicleTransform().GetRotation(),
		PhysicsGrappleComponent->GetTargetComponentTransfrom().GetRotation(),
		lerpFactor);*/
	
	
	//psudo on exit
	if (PhysicsGrappleComponent->ValidGrappleState() == false)
	{
		// sets velocity
		SphereComp->SetSimulatePhysics(true);
		FVector NewVel = PhysicsGrappleComponent->GetOnHookedDirection() * PhysicsGrappleComponent->GetOnHookedVelocitySize();
		SphereComp->SetPhysicsLinearVelocity(NewVel);

		//sets rotation speed
		CameraBoom->CameraLagSpeed = OnStartCameraLag.X;
		CameraBoom->CameraRotationLagSpeed = OnStartCameraLag.Y;
		
		
		EnterState(EVehicleState::AirBorne);
	}
	
}

void ACarPawn::StateAirBorne()
{
	if (bEnterState)
	{
		bEnterState = false;
		
	}
	SetUpVectorAsSplineUpAxis();
	RotateSphereCompToLocalUpVector();
	IsOutOfBounds();
	ApplyGravity();
	//SpeedHandleCameraBoomEffect(true);
	if (IsGrounded())
	{
		UE_LOG(LogTemp, Warning, TEXT("wohoo"))
		EnterState(EVehicleState::Driving);
	}

	//shoud we be in grapple state
	if (PhysicsGrappleComponent->ValidGrappleState())
	{
		EnterState(EVehicleState::Grappling);
	}
	
}

void ACarPawn::StateDashing()
{
	if (bEnterState)
	{
		bEnterState = false;
	}
}

void ACarPawn::ToggleGrappleHook()
{
	if (PhysicsGrappleComponent->GetCurrentGrappleState() == EGrappleStates::InActive)
	{
		//EnterState(EVehicleState::Grappling);
		PhysicsGrappleComponent->FireGrapplingHook();
	}
	else
	{
		// EnterState(EVehicleState::AirBorne);
		PhysicsGrappleComponent->RetractGrapplingHook();
	}
	
}

FVector ACarPawn::CalcAsymVector()
{
	float Angle = SignedAngleAxis(GetActorForwardVector(), 
		SphereComp->GetPhysicsLinearVelocity(), 
		GetActorUpVector());

	

	if (abs(Angle) > 90.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Did round down"));
		Angle = 0.f;
	}
	
	// DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() +
	// 	-GetActorRightVector() * Angle * 5.f , FColor::Cyan, false, 1.f);

	return -GetActorRightVector() * Angle * SphereComp->GetPhysicsLinearVelocity().Size();
}

float ACarPawn::CaltAsymForce()
{
	return 0.0f;
}

void ACarPawn::MoveXAxis(float Value)
{
	/*//comparing squared size since its faster
	if (IsUnderMaxSpeed(false) /*|| Value < 0.f#1#)
	{
		if (SphereComp->IsSimulatingPhysics())
		{
			float BreakForce = Value > 0.f ? 90000.f : 400000.f; 
			SphereComp->AddForce(GetActorForwardVector() * Value * BreakForce);
		}
	}*/
	Value = Value * UGameplayStatics::GetWorldDeltaSeconds(this);
	if (!SphereComp->IsSimulatingPhysics()) // guard cluase
		return;

	if (IsMovingForward()) // is moving forward
	{
		if (Value > 0.f) // accelerating
		{
			if (IsUnderMaxSpeed(false))
				SphereComp->AddForce(GetActorForwardVector() * AccelerationForce * Value, NAME_None, true);
			
		}
		else // deaccelerating
		{
			SphereComp->AddForce(GetActorForwardVector() * DeaccelerationForce * Value, NAME_None, true);
		}
	}
	else // moving bacward
	{
		if (Value > 0.f) // accelerating
		{
			SphereComp->AddForce(GetActorForwardVector() * AccelerationForce * Value, NAME_None, true);
		}
		else // still deaccelerating
		{
			if (IsUnderMaxSpeed(false))
				SphereComp->AddForce(GetActorForwardVector() * DeaccelerationForce * 0.2f * Value, NAME_None, true);
			
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("move!"));
}

void ACarPawn::MoveYAxis(float Value)
{
	//CarMesh->AddTorque(GetActorUpVector() * 1000000.f * Value);
	//SphereComp->AddRelativeRotation(FRotator(0.f, 1.f * Value, 0.f));
	CarMesh->AddRelativeRotation(FRotator(0.f, 1.f * Value * UGameplayStatics::GetWorldDeltaSeconds(this) * 100.f, 0.f));
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Value)

	FVector Forwardd = SphereComp->GetForwardVector();
	FVector Upp = SphereComp->GetUpVector();

	Forwardd =Forwardd.RotateAngleAxis(Value * TurnSpeed, Upp);

	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(Forwardd, Upp);
	SphereComp->SetWorldRotation(NewRot);


}

void ACarPawn::LookXAxis(float Value)
{
	FRotator OldRotation = CameraBoom->GetRelativeRotation();
	float Yaw = OldRotation.Yaw + CameraLookSpeed* Value * UGameplayStatics::GetWorldDeltaSeconds(this);
	Yaw = FMath::Clamp(Yaw, -MaxYawLookAngle, MaxYawLookAngle);

	OldRotation.Yaw = Yaw;
	CameraBoom->SetRelativeRotation(OldRotation);
}

void ACarPawn::LookYAxis(float Value)
{
	FRotator OldRotation = CameraBoom->GetRelativeRotation();
	float Pitch = OldRotation.Pitch + CameraLookSpeed* Value * UGameplayStatics::GetWorldDeltaSeconds(this);
	Pitch = FMath::Clamp(Pitch, -MaxPichLookAngle, MaxPichLookAngle);

	OldRotation.Pitch = Pitch;
	CameraBoom->SetRelativeRotation(OldRotation);
}

void ACarPawn::HandleBoost()
{
	if (IsUnderMaxSpeed(true) && SphereComp->IsSimulatingPhysics())
	{
		BoostComponent->Boost();
		CameraEffectComponent->PlayCameraEffect();
	}
}

/// <summary>
/// Returns angle in radians
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="axis"></param>
/// <returns></returns>
float ACarPawn::SignedAngleAxis(FVector v1, FVector v2, FVector axis)
{
	v1.Normalize();
	v2.Normalize();
	float DotProduct = FVector::DotProduct(v1, v2);
	float LengthCombined = (v1.Size() * v2.Size());
	float Angle = UKismetMathLibrary::Acos(DotProduct/ LengthCombined);
	//UE_LOG(LogTemp, Warning, TEXT("%f"),  FMath::RadiansToDegrees(Angle))
	
	FVector CrossProduct = FVector::CrossProduct(v1, v2);
	CrossProduct = CrossProduct.GetSafeNormal();
	float SignedFactor = FVector::DotProduct(CrossProduct, axis);

	Angle *= SignedFactor;
	Angle = FMath::RadiansToDegrees(Angle);
	return Angle;
	
}

float ACarPawn::UnsignedAngle(FVector v1, FVector v2)
{
	v1.Normalize();
	v2.Normalize();
	float DotProduct = FVector::DotProduct(v1, v2);
	float LengthCombined = (v1.Size() * v2.Size());
	float Angle = UKismetMathLibrary::Acos(DotProduct / LengthCombined);
	
	Angle = FMath::RadiansToDegrees(Angle);
	return Angle;
}

bool ACarPawn::IsGrounded()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	FHitResult hit{};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		ArrowRayCastStart->GetComponentLocation(),
		ArrowRayCastStart->GetComponentLocation() - GetActorUpVector() * 30.f,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);
	if (hit.IsValidBlockingHit() && UnsignedAngle(GravitySplineActive->GetAdjustedUpVectorFromLocation(SphereComp->GetComponentLocation()), hit.Normal) < MaxAngle) {
		//UE_LOG(LogTemp, Warning, TEXT("HIT"))
	
		
		return true;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("NO HIT"))
		return false;
	}

	
}

FVector ACarPawn::VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target)
{
	float Speed = UKismetMathLibrary::Cos(UnsignedAngle(Velocity, Target - StartLocation)) * Velocity.Size();
	//UE_LOG(LogTemp, Warning, TEXT("Angle - %f"), UnsignedAngle(Velocity, Target - StartLocation))
	FVector VelocityTowards = (Target - StartLocation).GetSafeNormal() * Speed;
	return VelocityTowards;
}

FHitResult ACarPawn::ShootRayFromCenterOfScreen()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	
	FVector Start = MainCamera->GetComponentLocation()/* + LocalUpVector * 250.f*/;
	FVector End = MainCamera->GetComponentLocation() + MainCamera->GetForwardVector() * 1000000.f;
	

	FHitResult hit{};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		Start,
		End,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

	// draw debug line
	//DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), hit.Location, FColor::Emerald, false, -1.f, (uint8)0U, 30.f);
	//if(hit.GetActor())
		//UE_LOG(LogTemp, Warning, TEXT("Hitting: %s"), *hit.GetActor()->GetName());
	return FHitResult();
}

void ACarPawn::SetUpVectorAsSplineUpAxis()
{
	LocalUpVector = GravitySplineActive->GetAdjustedUpVectorFromLocation(SphereComp->GetComponentLocation());
	
}

bool ACarPawn::IsMovingForward()
{
	const FVector VehicleForward = SphereComp->GetForwardVector();
	FVector VelocityDirection = VehicleForward;
	if (SphereComp->IsSimulatingPhysics())
	{
		VelocityDirection = SphereComp->GetPhysicsLinearVelocity().GetSafeNormal();
	}
	const float Angle = UnsignedAngle(VehicleForward, VelocityDirection);
	
	if (Angle <= 90.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Moving fowards!"))
		return true;
	}
	return false;
}

void ACarPawn::SpeedHandleCameraBoomEffect(bool bSoft)
{
	float speed = SphereComp->GetPhysicsLinearVelocity().Size();

	
	if (bSoft)
	{
		CameraBoom->TargetArmLength = StartCameraBoomLength + speed/30.f;
	}
	else
	{
		CameraBoom->TargetArmLength = StartCameraBoomLength + speed/90.f;
	}
}

bool ACarPawn::IsOutOfBounds()
{
	if (GravitySplineActive != nullptr)
	{
		FVector test = GravitySplineActive->SplineComp->
		FindLocationClosestToWorldLocation(SphereComp->GetComponentLocation(), ESplineCoordinateSpace::World);

		float dist = (test - SphereComp->GetComponentLocation()).Size();
		//UE_LOG(LogTemp, Warning, TEXT("Testing out of bounds%f"), dist)
		
		if (dist > 10000.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Resetting"))
			//SphereComp->SetWorldLocation(StartPlayerLocation);
		}
	}
	return false;
}

bool ACarPawn::IsUnderMaxSpeed(bool bBuffer)
{
	float BufferMaxSpeed = MaxSpeed * MaxSpeed;
	float buffer = 0.f;
	buffer = BufferMaxSpeed * 0.05f * bBuffer;


	if (BufferMaxSpeed + buffer > SphereComp->GetPhysicsLinearVelocity().SizeSquared())
	{
		
		//UE_LOG(LogTemp, Warning, TEXT(" awd %f Is under max speed"), GetWorld()->RealTimeSeconds)
		return true;
	}
	return false;
}

void ACarPawn::OnHitt(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/*LocalUpVector = GetActorLocation() - Hit.Location;
	LocalUpVector.Normalize();*/
	//LocalUpVector = Hit.Normal;
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() +  LocalUpVector * 14.f, FColor::Red, false, 1.f);
	//UE_LOG(LogTemp, Warning, TEXT("HITTTT! %s"), *OtherComp->GetName());
}

void ACarPawn::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AGravitySplineActor>())
	{
		GravitySplineActive = Cast<AGravitySplineActor>(OtherActor);
	}
	else if (OtherActor->IsA<AHighGravityZone>())
	{
		GravityMod = Cast<AHighGravityZone>(OtherActor)->GetGravityModifier();
		UE_LOG(LogTemp, Warning, TEXT("HIGH GRAV BABY"))
	}
}

void ACarPawn::OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	if (OtherActor->IsA<AHighGravityZone>())
	{
		GravityMod = BaseGravMod;
	}
}



