// Fill out your copyright notice in the Description page of Project Settings.


#include "GravitySplineActor.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "CarPawn.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
AGravitySplineActor::AGravitySplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW COMP"));
	SetRootComponent(ArrowComp);

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Gravity Spline Component"));
	SplineComp->SetupAttachment(ArrowComp);

	
}

// Called when the game starts or when spawned
void AGravitySplineActor::BeginPlay()
{
	Super::BeginPlay();
	SplineComp->bDrawDebug = true;
	
}

// Called every frame
void AGravitySplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerActor != nullptr)
	{
		
		int32 index = GetClosestSplinePoint(SplineComp, PlayerActor->GetActorLocation());
		ACarPawn* PlayerCar = Cast<ACarPawn>(PlayerActor);
		if (PlayerCar != nullptr)
		{
			PlayerCar->SphereComp->AddForce(SplineComp->GetUpVectorAtSplinePoint(index, ESplineCoordinateSpace::World) * -999999.f);

		}
		//UE_LOG(LogTemp, Warning, TEXT("PUSH!"))
	}

}

int32 AGravitySplineActor::GetClosestSplinePoint(USplineComponent* splineEval, FVector Location)
{
	
	float MaxDist = 9999999999.f;
	int index = -1;
	for (int32 i = 0; i < splineEval->GetNumberOfSplinePoints(); i++)
	{
		float Dist = (splineEval->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World) - Location).SizeSquared();
		if (Dist < MaxDist)
		{
			MaxDist = Dist;
			index = i;
		}
	}

	return index;

	FVector loc = splineEval->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);

	DrawDebugSphere(GetWorld(), loc , 70.f, 32,
		FColor::Emerald, false, 1.f);

	

	//FVector up = (splineEval->GetRotationAtSplinePoint(index, ESplineCoordinateSpace::World).Add(90.f, 0.f, 0.f)).Vector();
	FVector up = splineEval->GetUpVectorAtSplinePoint(index, ESplineCoordinateSpace::World);

	DrawDebugLine(GetWorld(), loc, loc + up * 400.f, FColor::Red, false);


	return index;
}

