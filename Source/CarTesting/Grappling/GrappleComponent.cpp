// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"

#include "GrappleTarget.h"
#include "Camera/CameraComponent.h"
#include "CarTesting/CarPawn.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"




// Sets default values for this component's properties
UGrappleComponent::UGrappleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	CarPawn = Cast<ACarPawn>(Owner);
	if (CarPawn == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("GrappleCOmponent needs to have CarPawn refrenced!"))

}


// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	
	//Sets visibility to false and empties array
	for(int32 i = 0; i < GrappleTargetsLastFrame.Num(); i++)
	{
		GrappleTargetsLastFrame[i]->SetVisbility(false);
	}
	GrappleTargetsLastFrame.Empty();
		
	// Hit decetion
	//refrence-> https://medium.com/@JoshLmao/ue4-ukismetsystemlibrary-sphereoverlapactors-aead8f94c3e4
	// actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Init(Owner, 1);
	TArray<FHitResult> HitResults;
	
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), CarPawn->MainCamera->GetComponentLocation(), CarPawn->MainCamera->GetComponentLocation() +
		CarPawn->MainCamera->GetForwardVector() * 10000.f, SphereOverlapRadius,
		ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResults, true);

	//logic if we hit something
	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		if (HitResults[i].Actor != nullptr && HitResults[i].Actor->IsA(AGrappleTarget::StaticClass()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%f  --   %s"), GetWorld()->GetTimeSeconds() ,*HitResults[i].Actor->GetName());
			AGrappleTarget* TempGrapTar = Cast<AGrappleTarget>(HitResults[i].Actor);
			TempGrapTar->SetVisbility(true);
			GrappleTargetsLastFrame.Add(TempGrapTar);
			
			
		}
	}


	//UE_LOG(LogTemp, Warning, TEXT("%d"), GrappleTargetsLastFrame.Num())
	
}

void UGrappleComponent::SetTarget(AActor* NewTarget)
{
	Target = NewTarget;
}

AActor* UGrappleComponent::GetTarget() const
{
	return Target;
}

void UGrappleComponent::MoveTowardsTarget()
{
	if (Target != nullptr)
	{
		// FVector NewLocation = FMath::VInterpConstantTo(GetOwner()->GetActorLocation(),
		// 	Target->GetActorLocation(), UGameplayStatics::GetWorldDeltaSeconds(this), 10000.f);
		FVector NewLocation = (Target->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
		GetOwner()->SetActorLocation(NewLocation * GrappleSpeed * UGameplayStatics::GetWorldDeltaSeconds(this) + GetOwner()->GetActorLocation());
		
	}
}

float UGrappleComponent::DistanceToTargetSqr()
{
	if (Target != nullptr && GetOwner() != nullptr)
	{
		return FVector::DistSquared(GetOwner()->GetActorLocation(), Target->GetActorLocation());
		
	}
	return -1.f;
}

