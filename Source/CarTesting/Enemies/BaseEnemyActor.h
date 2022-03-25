// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemyActor.generated.h"

UCLASS()
class CARTESTING_API ABaseEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEnemyActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
		class USphereComponent* SphereComp{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* CosmeticMest{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity|Spline")
		class AGravitySplineActor* GravitySplineActive{nullptr};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target|Player Pawn")
		class APawn* PlayerPawn{nullptr};
	
	UFUNCTION()
	virtual void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AGravitySplineActor* GetClosestGravitySpline();

	bool IsGrounded();

	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UArrowComponent* ArrowRayCastStart{ nullptr };

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
