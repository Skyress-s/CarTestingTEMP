// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"

#include <Actor.h>

#include "GrappleTarget.h"
#include "Camera/CameraComponent.h"
#include "CarTesting/CarPawn.h"
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
	// Owner = GetOwner();
	// CarPawn = Cast<ACarPawn>(Owner);
	// if (CarPawn == nullptr)
	// 	UE_LOG(LogTemp, Warning, TEXT("GrappleCOmponent needs to have CarPawn refrenced!"))

}


// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//refrence-> https://medium.com/@JoshLmao/ue4-ukismetsystemlibrary-sphereoverlapactors-aead8f94c3e4
	
	// //trace object types = world static
	// TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	// traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	//
	// // actors to ignore
	// TArray<AActor*> ActorsToIgnore;
	// ActorsToIgnore.Init(Owner, 1);
	//
	// // out actors
	// TArray<AActor*> OutActors;
	//
	//
	// TArray<FHitResult> HitResults;
	//
	// // UKismetSystemLibrary::SphereTraceMulti(GetWorld(), CarPawn->MainCamera->GetComponentLocation(), CarPawn->MainCamera->GetForwardVector() * 100000.f, SphereOverlapRadius,
	// // 	ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResults, true);
	
}
