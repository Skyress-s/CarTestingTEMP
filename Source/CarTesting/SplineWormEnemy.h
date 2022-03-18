// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineWormEnemy.generated.h"

UCLASS()
class CARTESTING_API ASplineWormEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineWormEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Spline")
	class USplineComponent* Spline = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
	class USplineMeshComponent* SplineMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
	class UStaticMeshComponent* WormTargetMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline", BlueprintReadOnly)
	class UGrappleSphereComponent* GrappleSphereComponent = nullptr;

	UFUNCTION(BlueprintCallable)
	void UpdateSplineMeshComponent();

	// UPROPERTY(EditAnywhere, Category = "SplineMesh")
	// float StartLength = 0.f;
	// UPROPERTY(EditAnywhere, Category = "SplineMesh")
	// float EndLength = 100.f;
	UPROPERTY(EditAnywhere, Category = "SplineMesh")
	float StaticMeshLength = 3000.f;
	UPROPERTY(EditAnywhere, Category = "SplineMesh")
	float Offset = 0.f;
	UPROPERTY(EditAnywhere, Category = "SplineMesh")
	float TangetLength = 1.f;
	UPROPERTY(EditAnywhere, Category = "SplineMesh")
	float WormMoveSpeed = 2000.f;
	
	UPROPERTY()
	bool bPlayingAnim = true;
private:
	UFUNCTION()
	void OnGrappleReaced(float Addspeed);
};
