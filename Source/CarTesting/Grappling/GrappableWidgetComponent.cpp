// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappableWidgetComponent.h"

#include "CarTesting/CarPawn.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UGrappableWidgetComponent::UGrappableWidgetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UGrappableWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	CarPawn = Cast<ACarPawn>(GetOwner());
	
	SetupWidget();
}


// Called every frame
void UGrappableWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentAttachComponent && WidgetComponent)
	{
		WidgetComponent->SetWorldLocation(CurrentAttachComponent->GetComponentLocation());
		
	}
}



void UGrappableWidgetComponent::PlayAnimation(USceneComponent* NewSceneComponent)
{
	if (WidgetComponent == nullptr)
	{
		SetupWidget();
	}
	UGrappableWidget* GWidget = Cast<UGrappableWidget>(WidgetComponent->GetWidget());
	if (GWidget)
	{
		CurrentAttachComponent = NewSceneComponent;
		GWidget->PlayCloseAnimationCpp();
		FTimerHandle Handle;

		FTimerDelegate Callback;
		Callback.BindLambda([this]
		{
			WidgetComponent->DestroyComponent();
			WidgetComponent = nullptr;
		});
		float Time = GWidget->WidgetAnim->GetEndTime();
		GetWorld()->GetTimerManager().SetTimer(Handle, Callback, Time, false);
	}
}

void UGrappableWidgetComponent::SetupWidget()
{
	WidgetComponent = NewObject<UWidgetComponent>(this);
	WidgetComponent->RegisterComponent();
	WidgetComponent->SetWidgetClass(GrappableWidget);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->AttachToComponent(CarPawn->SphereComp, FAttachmentTransformRules::KeepWorldTransform);
	WidgetComponent->SetRelativeLocation(FVector::ZeroVector);
}
