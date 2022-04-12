// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "BaseEnemyActor.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
}

void AEnemySpawner::OnSpawnerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (EnemyClassToSpawn && OtherActor->IsA<APawn>())
	{
		FRotator SpawnOrientation{OtherActor->GetActorRotation()};
		for (int i{}; i < NumberOfEnemiesToSpawn; i++)
		{
			EnemyActors[i] = GetWorld()->SpawnActor<ABaseEnemyActor>(EnemyClassToSpawn, FVector(0.f), SpawnOrientation);
		}
	}
}

void AEnemySpawner::OnSpawnerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnSpawnerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnSpawnerOverlapEnd);

	EnemyActors.Init(nullptr, NumberOfEnemiesToSpawn);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

