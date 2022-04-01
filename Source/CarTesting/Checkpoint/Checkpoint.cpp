// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"

#include "CarTesting/CarPawn.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("BaseArrow"));
	SetRootComponent(BaseArrow);

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainMesh->SetupAttachment(GetRootComponent());

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent(Trigger)"));
	BoxComponent->SetupAttachment(GetRootComponent());
	// boxcomponent is set to overlap all by defualt. So dont have to set this manually at this time in cpp

	SpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PlayerSpawnArrow"));
	SpawnArrow->SetupAttachment(GetRootComponent());

	
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	//setting up game mode
	AGameModeBase* BaseGameMode = GetWorld()->GetAuthGameMode();
	if (BaseGameMode->IsA(ACarTestingGameMode::StaticClass()))
		GameModee = Cast<ACarTestingGameMode>(BaseGameMode);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);
	
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap!"))
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		 if (GameModee)
			GameModee->SetLastCheckpoint(this);
	}
	
}

