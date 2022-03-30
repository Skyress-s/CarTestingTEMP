// Fill out your copyright notice in the Description page of Project Settings.


#include "TestingActor.h"

#include "Components/WidgetComponent.h"

// Sets default values
ATestingActor::ATestingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UWidgetComponent* widget = NewObject<UWidgetComponent>();
	widget->SetWidget(UserWidget.GetDefaultObject());
}

// Called when the game starts or when spawned
void ATestingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

