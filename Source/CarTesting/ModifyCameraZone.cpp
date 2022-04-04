// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifyCameraZone.h"

#include "CarPawn.h"
#include "Components/BoxComponent.h"

// Sets default values
AModifyCameraZone::AModifyCameraZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	SetRootComponent(BoxComponent);

}

// Called when the game starts or when spawned
void AModifyCameraZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AModifyCameraZone::OnOverLap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AModifyCameraZone::OnEndOverlap);
	
}

// Called every frame
void AModifyCameraZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AModifyCameraZone::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		ACarPawn* CarPawn = Cast<ACarPawn>(OtherActor);
		
		if (bAddative)
		{
			CarPawn->SetTargetCameraBoomLength(CarPawn->GetStartCameraBoomLength() + CameraBoomLength);	
		}
		else
		{
			CarPawn->SetTargetCameraBoomLength(CameraBoomLength);
		}
	}
	
}

void AModifyCameraZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		ACarPawn* CarPawn = Cast<ACarPawn>(OtherActor);
		//resets to origninal cameraBoomLength
		CarPawn->SetTargetCameraBoomLength(CarPawn->GetStartCameraBoomLength());
	}
}

