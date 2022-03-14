// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraEffecttComponent.h"

#include "Camera/CameraComponent.h"
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

	// ...
	if (CameraCurrent)
	{
		
		StartFOV = CameraCurrent->FieldOfView;
	}
}


// Called every frame
void UCameraEffecttComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bPlaying)
	{
		Time += UGameplayStatics::GetWorldDeltaSeconds(this);

		CameraCurrent->PostProcessSettings.SceneFringeIntensity = SpeedBoostCurve->GetFloatValue(Time) * 10.f;
		CameraCurrent->FieldOfView = StartFOV + SpeedBoostCurve->GetFloatValue(Time) * 20.f;
		
		

		if (Time > 3.f)
		{
			Time = 0.f;
			bPlaying = false;
		}
	}
}

void UCameraEffecttComponent::PlayCameraEffect()
{
	bPlaying = true;
}

