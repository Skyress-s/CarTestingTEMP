// Fill out your copyright notice in the Description page of Project Settings.


#include "NeckComponent.h"

#include <ThirdParty/openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathQuat.h>

#include "Components/SphereComponent.h"
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

	SplineMeshComponents.Init(nullptr, 0);
	
}


// Called every frame
void UNeckComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
}

void UNeckComponent::UpdateSplinePoints()
{
	FVector StartLocation, StartTangent, EndLocation, EndTangent = FVector::ZeroVector;
	
	StartLocation = CarPawn->CarMesh->GetComponentLocation();
	EndLocation = CarPawn->GrappleHookMesh->GetComponentLocation();

	float Distance = (StartLocation - EndLocation).Size();
	
	StartTangent = CarPawn->CarMesh->GetForwardVector() * Distance;
	EndTangent = CarPawn->GrappleHookMesh->GetForwardVector() * Distance;

	//new method for end tangen
	EndTangent = FVector::CrossProduct( CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(),
		CarPawn->SphereComp->GetForwardVector());
	EndTangent = EndTangent.GetSafeNormal();
	EndTangent *= Distance;
	
	//Sets the values to the spline
	
	Spline->SetLocationAtSplinePoint(0, StartLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
	Spline->SetLocationAtSplinePoint(1, EndLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World, true);
	
}



void UNeckComponent::UpdateSplinePointsLocations(FVector Start, FVector End, bool bLerp)
{
	if (bLerp)
	{
		FVector PrevStart = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector PrevEnd = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		Start = FMath::VInterpTo(PrevStart, Start, GetWorld()->GetDeltaSeconds(), 15.f);
		End = FMath::VInterpTo(PrevEnd, End, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetLocationAtSplinePoint(0, Start, ESplineCoordinateSpace::World, false);
	Spline->SetLocationAtSplinePoint(1, End, ESplineCoordinateSpace::World, true);
}

void UNeckComponent::UpdateSplineStartPosition(FVector Start)
{
	Spline->SetLocationAtSplinePoint(0, Start, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateSplineEndPosition(FVector End)
{
	Spline->SetLocationAtSplinePoint(1, End, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateSplinePointsTangents(FVector StartTangent, FVector EndTangent, bool bLerp)
{
	if (bLerp)
	{
		FVector PrevStartTangent = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector PrevEndTangent = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		StartTangent = FMath::VInterpTo(PrevStartTangent, StartTangent, GetWorld()->GetDeltaSeconds(), 15.f);
		EndTangent = FMath::VInterpTo(PrevEndTangent, EndTangent, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World, true);
	
}

void UNeckComponent::UpdateSplineMesh()
{
	UpdateSplineMesh(0, Spline->GetSplineLength());


	return;
	//adds or removes segments to the array
	int32 segments = CalculateNumberOfSegments();
	int32 SegmentsToCreate = segments - SplineMeshComponents.Num();
	if (SegmentsToCreate > 0)
	{
		for (int32 i = 0; i < SegmentsToCreate; i++)
		{
			USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
			if (NewSplineMesh)
			{
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(StaticMeshClass);
				SplineMeshComponents.Emplace(NewSplineMesh);
			}
		}
		
	}
	else if (SegmentsToCreate < 0)
	{
		for (int32 i = 0; i < abs(SegmentsToCreate); i++)
		{
			int32 LastIndex = SplineMeshComponents.Num() - 1;
			SplineMeshComponents[LastIndex]->DestroyComponent();
			SplineMeshComponents.RemoveAt(LastIndex);
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Current number of spline meshes! %d"), SplineMeshComponents.Num())

	//sets the positions and tangent of the spline mesh components
	float truncSplineLength = TargetSegmentsLength * SplineMeshComponents.Num();
	float lastLength = truncSplineLength;
	for (int32 i = 0; i < SplineMeshComponents.Num(); i++)
	{
		float currentLength = TargetSegmentsLength * i;

		currentLength = Spline->GetSplineLength() - currentLength;

		SplineMeshComponents[i]->SetStartPosition(Spline->GetLocationAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World),  false);
		
		SplineMeshComponents[i]->SetEndPosition(Spline->GetLocationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World), true);
		
		lastLength = currentLength;
	}
}

void UNeckComponent::UpdateSplineMesh(float StartLength, float EndLength)
{
	//clamps input
	if (StartLength < 0.f || StartLength > Spline->GetSplineLength()) StartLength = 0.f;
	if (EndLength > Spline->GetSplineLength() || EndLength < 0.f) EndLength = Spline->GetSplineLength();
	//adds or removes segments to the array
	//int32 segments = CalculateNumberOfSegments();
	float Length = EndLength - StartLength;
	float segmentsf = Length / TargetSegmentsLength;
	int32 segments = (int)truncf(segmentsf) + 1;
	int32 SegmentsToCreate = segments - SplineMeshComponents.Num();
	
	
	if (SegmentsToCreate > 0)
	{
		for (int32 i = 0; i < SegmentsToCreate; i++)
		{
			USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
			if (NewSplineMesh)
			{
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(StaticMeshClass);
				SplineMeshComponents.Emplace(NewSplineMesh);
			}
		}
		
	}
	else if (SegmentsToCreate < 0)
	{
		for (int32 i = 0; i < abs(SegmentsToCreate); i++)
		{
			int32 LastIndex = SplineMeshComponents.Num() - 1;
			SplineMeshComponents[LastIndex]->DestroyComponent();
			SplineMeshComponents.RemoveAt(LastIndex);
		}
	}

	//sets the positions and tangets
	//float truncSplineLength = TargetSegmentsLength * SplineMeshComponents.Num();
	float lastLength = EndLength;
	for (int32 i = 0; i < SplineMeshComponents.Num(); i++)
	{
		float currentLength = TargetSegmentsLength * i;

		currentLength = Length - currentLength;
		currentLength += StartLength;

		SplineMeshComponents[i]->SetStartPosition(Spline->GetLocationAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World),  false);
		
		SplineMeshComponents[i]->SetEndPosition(Spline->GetLocationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World), true);
		
		lastLength = currentLength;

		/*if (i == 3)
			break;*/
	}
}

int32 UNeckComponent::CalculateNumberOfSegments()
{
	float Length = Spline->GetSplineLength();

	float segmentsf = Length/TargetSegmentsLength;
	int32 segments = (int)truncf(segmentsf);

	//UE_LOG(LogTemp, Warning, TEXT("%d"), segments)

	return segments;
}



