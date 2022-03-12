// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostComponent.h"
#include "CarPawn.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UBoostComponent::UBoostComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UBoostComponent::BeginPlay()
{
	Super::BeginPlay();

	CarPawn = Cast<ACarPawn>(GetOwner());
	
}


// Called every frame
void UBoostComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*if (PhysComp != nullptr)
	{
		PhysComp->AddForce(GetOwner()->GetActorUpVector() * 7000000.f);
	}*/
	if (bBoosting)
	{
		float Force = BoostCurve->GetFloatValue(CurrentBoostTime);
		CurrentBoostTime += DeltaTime/BoostDuration;
		
		PhysComp->AddForce(GetOwner()->GetActorForwardVector() * 100000.f * Force);
		UE_LOG(LogTemp, Warning, TEXT("Is boosting - %f"), Force)
	}
}

void UBoostComponent::Boost()
{
	// if (PhysComp != nullptr && CarPawn->IsUnderMaxSpeed(true))
	// {
	// 	PhysComp->AddImpulse(GetOwner()->GetActorForwardVector() * 100000.f);
	// 	/*UE_LOG(LogTemp, Warning, )*/
	// }

	if (PhysComp != nullptr && CarPawn->IsUnderMaxSpeed(true))
	{
		bBoosting = true;
		GetWorld()->GetTimerManager().SetTimer(BoostDurationTimerHandle,this, &UBoostComponent::Disable_bBoosting,
			BoostDuration, false);
		CurrentBoostTime = 0.f;
	}
	
}

