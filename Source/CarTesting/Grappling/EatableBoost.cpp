// Fill out your copyright notice in the Description page of Project Settings.


#include "EatableBoost.h"

// Sets default values
AEatableBoost::AEatableBoost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AEatableBoost::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEatableBoost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

