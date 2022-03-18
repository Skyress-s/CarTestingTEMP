// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineWormEnemy.h"

#include "GrappleSphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASplineWormEnemy::ASplineWormEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);
	SplineMeshComponent = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineMesh"));
	SplineMeshComponent->SetupAttachment(GetRootComponent());

	WormTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormTargetMesh"));
	WormTargetMesh->SetupAttachment(GetRootComponent());
	WormTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(WormTargetMesh);
	
	
}

// Called when the game starts or when spawned
void ASplineWormEnemy::BeginPlay()
{
	Super::BeginPlay();
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ASplineWormEnemy::OnGrappleReaced);
}

// Called every frame
void ASplineWormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bPlayingAnim)
		return;


	Offset += -DeltaTime * WormMoveSpeed;
	if (Offset <= 0.f)
		Offset = 0.f;
	
	UpdateSplineMeshComponent();
}

void ASplineWormEnemy::UpdateSplineMeshComponent()
{
	FVector StartPos;
	FVector StartTangent;
	FVector EndPos;
	FVector EndTangent;

	float SplineLength = Spline->GetSplineLength();

	//setting and clamping locations and tangents
	StartPos = Spline->GetLocationAtDistanceAlongSpline(Offset, ESplineCoordinateSpace::World);
	StartTangent = Spline->GetTangentAtDistanceAlongSpline(Offset, ESplineCoordinateSpace::World);
	StartTangent *= TangetLength;
	EndPos = Spline->GetLocationAtDistanceAlongSpline(Offset + StaticMeshLength, ESplineCoordinateSpace::World);
	EndTangent = Spline->GetTangentAtDistanceAlongSpline(Offset + StaticMeshLength, ESplineCoordinateSpace::World);
	EndTangent *= TangetLength;

	//sets the spline mesh values
	SplineMeshComponent->SetStartPosition(StartPos, true);
	SplineMeshComponent->SetStartTangent(StartTangent, true);
	SplineMeshComponent->SetEndPosition(EndPos, true);
	SplineMeshComponent->SetEndTangent(EndTangent, true);

	//updates the WormTargetMesh
	float WormTargetSplineLength = StaticMeshLength/2.f;
	FVector WormTargetMeshLocation = Spline->GetLocationAtDistanceAlongSpline(Offset + WormTargetSplineLength, ESplineCoordinateSpace::World);
	FRotator WormTargetMeshRotation = UKismetMathLibrary::MakeRotFromXZ(
		Spline->GetDirectionAtDistanceAlongSpline(Offset + WormTargetSplineLength, ESplineCoordinateSpace::World),
		Spline->GetUpVectorAtDistanceAlongSpline(Offset + WormTargetSplineLength, ESplineCoordinateSpace::World)
	);
	
	WormTargetMesh->SetWorldLocation(WormTargetMeshLocation);
	WormTargetMesh->SetWorldRotation(WormTargetMeshRotation);
}

void ASplineWormEnemy::OnGrappleReaced(float Addspeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed Worm"))
	Destroy();
}


