// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"

#include "GrappleTarget.h"
#include "Camera/CameraComponent.h"
#include "CarTesting/CarPawn.h"
#include "Components/WidgetComponent.h"
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
		
	
	//refrence-> https://medium.com/@JoshLmao/ue4-ukismetsystemlibrary-sphereoverlapactors-aead8f94c3e4
	// actors to ignore
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Init(Owner, 1);
	TArray<FHitResult> HitResults;
	
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), CarPawn->MainCamera->GetComponentLocation(), CarPawn->MainCamera->GetComponentLocation() +
		CarPawn->MainCamera->GetForwardVector() * 10000.f, SphereOverlapRadius,
		ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResults, true);

	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		if (HitResults[i].Actor != nullptr && HitResults[i].Actor->IsA(AGrappleTarget::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%f  --   %s"), GetWorld()->GetTimeSeconds() ,*HitResults[i].Actor->GetName());
			AGrappleTarget* TempGrapTar = Cast<AGrappleTarget>(HitResults[i].Actor);
			TempGrapTar->SetVisbility(true);
			GrappleTargetsLastFrame.Add(TempGrapTar);
			
		}
	}
}
