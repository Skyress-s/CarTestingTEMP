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
#include "GravitySplineActor.h"
#include "HighGravityZone.h"
#include "PhysicsGrapplingComponent.h"
#include "CollisionAnalyzer/Public/ICollisionAnalyzer.h"
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

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	GetComponents<UPrimitiveComponent>(PrimitiveComponents, false /*or true*/);
	UE_LOG(LogTemp, Warning, TEXT("%d"), PrimitiveComponents.Num())
		BoostComponent->PhysComp = PrimitiveComponents[0];
	UE_LOG(LogTemp, Warning, TEXT("%s"), *PrimitiveComponents[0]->GetName())
	
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
			static
			float HeightAboveGround{DistanceToGround()};
			UE_LOG(LogTemp, Warning, TEXT("Height %f"), HeightAboveGround);
			FVector HeightVelocity{
				FVector::DotProduct(SphereComp->GetPhysicsLinearVelocity(),GravityUpVector)*GravityUpVector
			};
			FVector GravityForce{-GravityUpVector * 68.1f * 40.f * GravityMod};
			FVector HoverForce{
				-GravityForce*UKismetMathLibrary::Exp(1.f - HeightAboveGround*0.02) -
					HeightVelocity*UKismetMathLibrary::Exp(1.f - HeightAboveGround*0.02)
			};
			SphereComp->AddForce(GravityForce+HoverForce, FName(), true);
		}
	}
}

void ACarPawn::TiltCarMesh(FVector AsymVector)
{
	//orients the mesh
	FRotator NewRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector + AsymVector * 0.0001f,
	                                                    GetActorForwardVector());
	CarMesh->SetWorldRotation( FMath::RInterpTo(CarMesh->GetComponentRotation(),
	                                            NewRot,
	                                            UGameplayStatics::GetWorldDeltaSeconds(this),
	                                            5.f
	) );
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
	FInputActionBinding& action = PlayerInputComponent->BindAction("Boost", EInputEvent::IE_Pressed, BoostComponent, &UBoostComponent::Boost);
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
		//PhysicsGrappleComponent->FireGrapplingHook();
		//EnterState(EVehicleState::Driving);
		SphereComp->SetSimulatePhysics(false);
	}

	//psudo on exit
	if (PhysicsGrappleComponent->ValidGrappleState() == false)
	{
		SphereComp->SetSimulatePhysics(true);
		FVector NewVel = PhysicsGrappleComponent->GetOnHookedDirection() * PhysicsGrappleComponent->GetOnHookedVelocitySize();
		SphereComp->SetPhysicsLinearVelocity(NewVel);
		
		EnterState(EVehicleState::AirBorne);
	}
	/*// if (bEnterState)
	// {
	// 	if (GrappleComponent->GrappleTargetsLastFrame.Num() > 0)
	// 	{
	// 		//UE_LOG(LogTemp, Warning, TEXT("%s"), *GrappleComponent->GrappleTargetsLastFrame[0]->GetOwner()->GetName())
	// 		if (GrappleComponent->GrappleTargetsLastFrame[0] != nullptr)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("%d"), GrappleComponent->GrappleTargetsLastFrame.Num())
	// 			GrappleComponent->SetTarget(GrappleComponent->GrappleTargetsLastFrame[0]);
	// 			GrappleComponent->SetGrappleSpeed(SphereComp->GetPhysicsLinearVelocity().Size());
	// 			GrappleComponent->SetDirectionAtStart((GrappleComponent->GetTarget()->GetActorLocation() - GetActorLocation()).GetSafeNormal());
	// 			SphereComp->SetSimulatePhysics(false);
	// 		}
	// 		else
	// 		{
	// 			EnterState(EVehicleState::AirBorne);
	// 		}
	// 		
	// 	}
	// 	else
	// 	{
	// 		EnterState(EVehicleState::AirBorne);
	// 	}
	// }
	//
	//
	// GrappleComponent->MoveTowardsTarget();
	//
	// if (GrappleComponent->DistanceToTargetSqr() < FinishGrappleDistance * FinishGrappleDistance)
	// {
	// 	SphereComp->SetSimulatePhysics(true);
	// 	SphereComp->SetPhysicsLinearVelocity(GrappleComponent->GetGrappleSpeed() * GrappleComponent->GetDirectionAtStart());
	// 	EnterState(EVehicleState::AirBorne);
	//
	// 	//rotates the sphere mesh to appropiate
	// 	FRotator NewSphereRot = UKismetMathLibrary::MakeRotFromXZ(GrappleComponent->GetDirectionAtStart(), LocalUpVector);
	// 	SphereComp->SetWorldRotation(NewSphereRot);
	// 	SphereComp->SetPhysicsAngularVelocity(FVector::ZeroVector);
	// }*/
}

void ACarPawn::StateAirBorne()
{
	if (bEnterState)
	{
		bEnterState = false;
		
	}
	ApplyGravity();
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
	//comparing squared size since its faster
	if (IsUnderMaxSpeed(false) || Value < 0.f)
	{
		if (SphereComp->IsSimulatingPhysics())
		{
			SphereComp->AddForce(GetActorForwardVector() * Value * 70000.f);
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

	Forwardd =Forwardd.RotateAngleAxis(Value * 1.f, Upp);

	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(Forwardd, Upp);
	SphereComp->SetWorldRotation(NewRot);



}

void ACarPawn::LookXAxis(float Value)
{
	CameraBoom->AddRelativeRotation(FRotator(0.f, Value, 0.f));
	//UE_LOG(LogTemp, Warning, TEXT("Looking %f"), Value)
}

void ACarPawn::LookYAxis(float Value)
{
	CameraBoom->AddRelativeRotation(FRotator( Value, 0.f,  0.f));
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
		UE_LOG(LogTemp, Warning, TEXT("HIT %f"), (hit.ImpactPoint - ArrowRayCastStart()->GetComponentLocation()).Size());
	
		
		return true;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("NO HIT"))
		return false;
	}

	
}

float ACarPawn::DistanceToGround()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	FHitResult hit{};
	FVector StartLocation{ArrowRayCastStart->GetComponentLocation()};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		StartLocation,
		StartLocation - GetActorUpVector() * 30.f,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);
	if (hit.IsValidBlockingHit()) {
		//UE_LOG(LogTemp, Warning, TEXT("HIT"))
		return (hit.ImpactPoint - StartLocation).Size();
	
		
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



