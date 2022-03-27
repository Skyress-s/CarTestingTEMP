// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEatableActor.h"

#include "CarTesting/GrappleSphereComponent.h"

// Sets default values
ABasicEatableActor::ABasicEatableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MainMesh);

	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());
	GrappleSphereComponent->SetIsEatable(true);
	
		
}

// Called when the game starts or when spawned
void ABasicEatableActor::BeginPlay()
{
	Super::BeginPlay();

	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ABasicEatableActor::OnReached);
}

// Called every frame
void ABasicEatableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicEatableActor::OnReached(float AddSpeedAmount)
{
	Destroy();
}

