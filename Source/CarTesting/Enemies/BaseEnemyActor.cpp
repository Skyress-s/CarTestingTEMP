// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyActor.h"

#include "CarTesting/GravitySplineActor.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseEnemyActor::ABaseEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// collider:
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSimulatePhysics(false);
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// cosmetic mesh:
	CosmeticMest = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CosmeticMest->SetupAttachment(GetRootComponent());
	CosmeticMest->SetSimulatePhysics(false);
	CosmeticMest->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// arrow
	ArrowRayCastStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow RayCastStart"));
	ArrowRayCastStart->SetupAttachment(GetRootComponent());
}

void ABaseEnemyActor::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AGravitySplineActor>())
	{
		GravitySplineActive = Cast<AGravitySplineActor>(OtherActor);
	}
}

// Called when the game starts or when spawned
void ABaseEnemyActor::BeginPlay()
{
	Super::BeginPlay();
}

AGravitySplineActor* ABaseEnemyActor::GetClosestGravitySpline()
{
	TArray<AActor*> GravitySplines;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravitySplineActor::StaticClass(), GravitySplines);
	}
	float MinDistance{};
	int32 MinIndex{};
	for (int32 i{}; i < GravitySplines.Num(); i++)
	{
		float TempDistance{(GravitySplines[i]->GetActorLocation() - GetActorLocation()).Size()};
		if (i == 0)
		{
			MinDistance = TempDistance;
		}
		else if (MinDistance > TempDistance)
		{
			MinDistance = TempDistance;
			MinIndex = i;
		}
	}
	return Cast<AGravitySplineActor>(GravitySplines[MinIndex]);
}

bool ABaseEnemyActor::IsGrounded()
{
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

		FHitResult hit{};
		GetWorld()->LineTraceSingleByObjectType(
			hit,
			ArrowRayCastStart->GetComponentLocation(),
			ArrowRayCastStart->GetComponentLocation() - GetActorUpVector() *1.05* SphereComp->GetScaledSphereRadius(),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			TraceParams
		);
		if (hit.IsValidBlockingHit()) {
			//UE_LOG(LogTemp, Warning, TEXT("HIT"))
			return true;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("NO HIT"))
			return false;
		}
}

// Called every frame
void ABaseEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PlayerPawn) PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}

