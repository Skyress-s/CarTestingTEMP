// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraEffecttComponent.h"

#include "CarPawn.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraModifier.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCameraEffecttComponent::UCameraEffecttComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraEffecttComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
	if (GetOwner()->IsA(ACarPawn::StaticClass()))
		CarPawn = Cast<ACarPawn>(GetOwner());

	CameraCurrent = CarPawn->MainCamera;
	if (CameraCurrent)
		StartFOV = CameraCurrent->FieldOfView;


	// this is all the setup you ned for the boost modifier, it will de activate itself when being enabled
	BoostCameraModifier = UGameplayStatics::GetPlayerCameraManager(this, 0)->AddNewCameraModifier(BoostCameraModifierClass);
	BoostCameraModifier->DisableModifier(true);
}


// Called every frame
void UCameraEffecttComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	float Speed = 0.f;
	if (CarPawn->SphereComp->IsSimulatingPhysics())
	{
		Speed = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size();
		
	}
	else
	{
		Speed = 10000.f;
	}
	float TargetFOV = Speed / 300.f + StartFOV;
	TargetFOV = FMath::Clamp(TargetFOV, StartFOV, StartFOV * 2.f);

	TargetFOV = FMath::FInterpTo(CameraCurrent->FieldOfView, TargetFOV, GetWorld()->GetDeltaSeconds(), 5.f);
	
	CameraCurrent->SetFieldOfView(TargetFOV);
}


