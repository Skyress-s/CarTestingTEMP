// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleTarget.h"

#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h" 

// Sets default values
AGrappleTarget::AGrappleTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComp"));
	// SetRootComponent(BillboardComponent);
	//
	// Widgett = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	// Widgett->SetupAttachment(GetRootComponent());

	// SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// SphereComponent->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AGrappleTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrappleTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

