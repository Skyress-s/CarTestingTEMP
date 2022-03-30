// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
#include "Components/ActorComponent.h"
#include "Components/SplineMeshComponent.h"
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
friend UPhysicsGrapplingComponent;
	
private:

	UPROPERTY()
		ACarPawn* CarPawn = nullptr;
	//could use carpawn refrence, but going to use the spline alot, so this is shorthand
	UPROPERTY(/*meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Neck"*/)
		class USplineComponent* Spline = nullptr;

	// functions
	UFUNCTION()
	int32 CalculateNumberOfSegments();
	


	//variables
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Neck" )
		float TargetSegmentsLength = 100.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Neck")
		class UStaticMesh* StaticMeshClass = nullptr;
	
	UPROPERTY()
	TArray<USplineMeshComponent*> SplineMeshComponents;
protected:
	// Neck handling
	UFUNCTION()
	void UpdateSplinePoints();
	void UpdateSplinePointsLocations(FVector Start, FVector End, bool bLerp);
	void UpdateSplineStartPosition(FVector Start);
	void UpdateSplineEndPosition(FVector End);
	
	void UpdateSplinePointsTangents(FVector StartTangent, FVector EndTangent, bool bLerp);
	
	UFUNCTION()
	void UpdateSplineMesh();
	void UpdateSplineMesh(float StartLength, float EndLength);
};


