// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravitySplineActor.generated.h"

UCLASS()
class CARTESTING_API AGravitySplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravitySplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// my deseg --------
public:
	

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ArrowComp = nullptr;
	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComp = nullptr;
	

	UPROPERTY(EditAnywhere)
		AActor* PlayerActor = nullptr;


	UFUNCTION()
		int32 GetClosestSplinePoint(class USplineComponent* splineEval, FVector Location);
	

};
