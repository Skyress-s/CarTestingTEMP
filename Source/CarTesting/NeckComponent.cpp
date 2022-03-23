// Fill out your copyright notice in the Description page of Project Settings.


#include "NeckComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values for this component's properties
UNeckComponent::UNeckComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNeckComponent::BeginPlay()
{
	Super::BeginPlay();

	CarPawn = Cast<ACarPawn>(GetOwner());
	Spline = CarPawn->NeckSpline;
	// ...
	
}


// Called every frame
void UNeckComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNeckComponent::UpdateSplinePoints()
{
	FVector StartLocation, StartTangent, EndLocation, EndTangent = FVector::ZeroVector;
	
	StartLocation = CarPawn->CarMesh->GetComponentLocation();
	EndLocation = CarPawn->GrappleHookMesh->GetComponentLocation();

	float Distance = (StartLocation - EndLocation).Size();
	
	EndTangent = CarPawn->GrappleHookMesh->GetForwardVector() * Distance;
	StartTangent = CarPawn->CarMesh->GetForwardVector() * Distance;
	
	//Sets the values to the spline
	
	Spline->SetLocationAtSplinePoint(0, StartLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
	Spline->SetLocationAtSplinePoint(1, EndLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World, true);

	//TODO TEMP FOR TESTING MOVE TO UPDATE SPLINE MESH!
	CarPawn->NeckSplineMesh->SetStartPosition(StartLocation, false);
	CarPawn->NeckSplineMesh->SetStartTangent(StartTangent, false);
	CarPawn->NeckSplineMesh->SetEndPosition(EndLocation, false);
	CarPawn->NeckSplineMesh->SetEndTangent(EndTangent, true);
	
	CalculateNumberOfSegments();
}

void UNeckComponent::UpdateSplineMesh()
{
}

int UNeckComponent::CalculateNumberOfSegments()
{
	float Length = Spline->GetSplineLength();

	float segmentsf = Length/TargetSegmentsLength;
	int segments = truncf(segmentsf);

	UE_LOG(LogTemp, Warning, TEXT("%d"), segments)

	return segments;
}

