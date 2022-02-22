// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPawn.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Camera/CameraComponent.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"


// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Car Mesh"));
	CarMesh->SetupAttachment(GetRootComponent());
	CarMesh->SetSimulatePhysics(false);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainCamera->SetupAttachment(CameraBoom);

}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->OnComponentHit.AddDynamic(this, &ACarPawn::OnHit);
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsGrounded())
	{
		SphereComp->AddForce(CalcAsymVector() * 100000.f);


		// hovering

		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

		FHitResult hit{};
		GetWorld()->LineTraceSingleByObjectType(
			hit,
			GetActorLocation(),
			GetActorLocation() - GetActorUpVector() * 100.f,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			TraceParams
		);

		if (hit.IsValidBlockingHit())
		{
			/*float Force = (hit.Location - GetActorLocation()).Size();
			CarMesh->AddForce(hit.Normal * Force * 30.f * CarMesh->GetMass());

			FVector Vel = VelocityTowardsTarget(GetActorLocation(), CarMesh->GetPhysicsLinearVelocity(),
				hit.Location);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Vel * 10.f, FColor::Red, false,
				1.f);*/
			//UE_LOG(LogTemp, Warning, TEXT("Speed - %f"), Vel.Size())
			//CarMesh->AddForce(hit.Normal * Vel.Size() * 40.f * CarMesh->GetMass());
		}

	}
	
}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACarPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACarPawn::MoveYAxis);
}

FVector ACarPawn::CalcAsymVector()
{
	float Angle = SignedAngleAxis(GetActorForwardVector(), 
		SphereComp->GetPhysicsLinearVelocity(), 
		GetActorUpVector());

	

	if (abs(Angle) > 90.f)
	{
		Angle = 0.f;
	}

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() +
		-GetActorRightVector() * Angle * 1.f, FColor::Cyan, false, 1.f);

	return -GetActorRightVector() * Angle;
}

float ACarPawn::CaltAsymForce()
{
	return 0.0f;
}

void ACarPawn::MoveXAxis(float Value)
{
	SphereComp->AddForce(GetActorForwardVector() * Value * 1000000.f);
	//UE_LOG(LogTemp, Warning, TEXT("move!"));
}

void ACarPawn::MoveYAxis(float Value)
{
	//CarMesh->AddTorque(GetActorUpVector() * 1000000.f * Value);
	SphereComp->AddRelativeRotation(FRotator(0.f, 1.f * Value, 0.f));
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
		GetActorLocation(),
		GetActorLocation() - GetActorUpVector() * 100.f,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

	if (hit.IsValidBlockingHit()) {
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

void ACarPawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("HITTTT!"))
}

