// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityPoints.h"

// Sets default values
AGravityPoints::AGravityPoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Father = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AWdawd"));
	SetRootComponent(Father);
}

// Called when the game starts or when spawned
void AGravityPoints::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGravityPoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<USceneComponent*> Childrens{};
	Father->GetChildrenComponents(false, Childrens);

	UE_LOG(LogTemp, Warning, TEXT("%d"), Childrens.Num())

}

