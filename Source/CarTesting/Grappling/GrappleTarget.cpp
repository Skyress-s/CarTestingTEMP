// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleTarget.h"

#include "CarTesting/GrappleSphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h" 

// Sets default values
AGrappleTarget::AGrappleTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	
	RootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrow"));
	SetRootComponent(RootArrow);

	MainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainSkeletalMesh"));
	MainMesh->SetupAttachment(GetRootComponent());

	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(MainMesh, FName("Grapple"));
	GrappleSphereComponent->SetIsEnabled(false);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Box Component"));
	Trigger->SetupAttachment(GetRootComponent());
	Trigger->SetBoxExtent(FVector(200.f));
	
	
}

// Called when the game starts or when spawned
void AGrappleTarget::BeginPlay()
{
	Super::BeginPlay();
	
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &AGrappleTarget::OnReachedTarget);
	GrappleSphereComponent->OnGrappleHitEvent.AddDynamic(this, &AGrappleTarget::OnGrappleTarget);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGrappleTarget::OnTriggerEnter);
	
}

// Called every frame
void AGrappleTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GrappleSphereComponent->SetRelativeLocation(FVector::ZeroVector);
	

}

void AGrappleTarget::SetVisbility(bool bVisible)
{
	//Widget->SetVisibility(bVisible);
}

void AGrappleTarget::OnReachedTarget(float AddSpeedAmount)
{
	//Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Reached Grapple Target"))
}

void AGrappleTarget::OnGrappleTarget(FTransform SphereCompTransfrom)
{
	UE_LOG(LogTemp, Warning, TEXT("Grapple Grapple target"))
}

void AGrappleTarget::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MainMesh->Play(false);
	GrappleSphereComponent->SetIsEnabled(true);
	UE_LOG(LogTemp, Warning, TEXT("(GrappleTarget)Activated GrappleTarget"))
}

