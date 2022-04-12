// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappableWidgetComponent.h"

#include "CarTesting/CarPawn.h"
#include "CarTesting/PhysicsGrapplingComponent.h"
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

	//binding events
	CarPawn->PhysicsGrappleComponent->FoundHomingTargetEvent.AddDynamic(this, &UGrappableWidgetComponent::PlaceWidget);
	CarPawn->PhysicsGrappleComponent->BeginHomingEvent.AddDynamic(this, &UGrappableWidgetComponent::PlayAnimation);
	CarPawn->PhysicsGrappleComponent->LostHomingTargetEvent.AddDynamic(this, &UGrappableWidgetComponent::DeleteWidget);
	
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



void UGrappableWidgetComponent::PlayAnimation()
{
	if (WidgetComponent == nullptr)
	{
		SetupWidget();
	}
	
	UGrappableWidget* GWidget = Cast<UGrappableWidget>(WidgetComponent->GetWidget());
	if (GWidget)
	{
		
		GWidget->PlayCloseAnimationCpp();
		FTimerHandle Handle;

		FTimerDelegate Callback;
		Callback.BindLambda([this]
		{
			if (WidgetComponent)
			{
				WidgetComponent->DestroyComponent();
				WidgetComponent = nullptr;
			}
		});
		float Time = GWidget->WidgetAnim->GetEndTime();
		GetWorld()->GetTimerManager().SetTimer(Handle, Callback, Time, false);
	}
}

void UGrappableWidgetComponent::PlaceWidget(USceneComponent* NewSceneComponent)
{
	if (WidgetComponent == nullptr)
	{
		SetupWidget();
	}
	
	CurrentAttachComponent = NewSceneComponent;
}

void UGrappableWidgetComponent::DeleteWidget()
{
	if (WidgetComponent)
	{
		WidgetComponent->DestroyComponent();
		WidgetComponent = nullptr;
	}
}

void UGrappableWidgetComponent::SetupWidget()
{
	WidgetComponent = NewObject<UWidgetComponent>(this);
	WidgetComponent->RegisterComponent();
	WidgetComponent->SetWidgetClass(GrappableWidget);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//WidgetComponent->SetDrawSize(FVector2D(150, 150));
	WidgetComponent->AttachToComponent(CarPawn->SphereComp, FAttachmentTransformRules::KeepWorldTransform);
	WidgetComponent->SetRelativeLocation(FVector::ZeroVector);
	// WidgetComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	WidgetComponent->GetWidget()->SetRenderScale(FVector2D(WidgetSize,WidgetSize));
}
