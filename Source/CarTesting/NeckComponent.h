// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
#include "Components/ActorComponent.h"
#include "NeckComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UNeckComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNeckComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//My deseg

private:

	UPROPERTY()
		ACarPawn* CarPawn = nullptr;
	//could use carpawn refrence, but going to use the spline alot, so this is shorthand
	UPROPERTY(/*meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Neck"*/)
		class USplineComponent* Spline = nullptr;

	// functions
	UFUNCTION()
	int CalculateNumberOfSegments();


	//variables
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Neck" )
		float TargetSegmentsLength = 1000.f;
	
	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponents; 
public:
	// Neck handling
	UFUNCTION()
	void UpdateSplinePoints();
	UFUNCTION()
	void UpdateSplineMesh();
};
