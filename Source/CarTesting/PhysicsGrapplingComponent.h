// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
#include "GrappleSphereComponent.h"
#include "Components/ActorComponent.h"

#include "Enums/Enums.h"

#include "PhysicsGrapplingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UPhysicsGrapplingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhysicsGrapplingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//my deseg ------------
private:
	UPROPERTY()
		class ACarPawn* CarPawn = nullptr;
	UPROPERTY()
		FVector StartLocationGrappleMesh;
	UPROPERTY()
		EGrappleStates CurrentGrappleState = EGrappleStates::InActive;
	UPROPERTY()
		bool bEnterState = false;
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple")
		float FireGrappleSpeed = 19000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple")
		float GrappleRotationSpeed = 500.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple")
		float MaxGrappleDistance = 15000.f; 
	
	//hooked

	//caps how slow or fast Player shoudl move
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapp|OnHooked")
		float LowestOnHookedSpeed = 2000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapp|OnHooked")
		float HighestOnHookedSpeed = 9000.f;

	UPROPERTY()
		float OnHookedSpeed = 0.f;
	UPROPERTY()
		FVector OnHookedDirection = FVector::ZeroVector;
	UPROPERTY()
		FTransform OnHookedVehicleTransfrom = FTransform::Identity;
	
	UPROPERTY()
		float MoveToTargetModifier = 1.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
		float MoveToTargetAcceleration = 10.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple|Hooked")
		UCurveFloat* HookedMovementCurve = nullptr;
	UPROPERTY()
		float CurrentHookedTime = 0.f; 

	UPROPERTY(/*meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple"*/)
		UGrappleSphereComponent* TargetGrappableComponent = nullptr;

	// OnHooked eatable
	UPROPERTY()
		class UGrappleSphereComponent* EatableGrappleSphereComponent = nullptr;

	//returning
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple|Returning")
		float ReturnTime = 1.f;
	UPROPERTY()
		float  CurrentReturnTime = 0.f;

	// returning with eatable
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Grapple|Eatable")
		float ReturnEatableTime = 0.4f;
public:
	UFUNCTION()
		EGrappleStates GetCurrentGrappleState(){return CurrentGrappleState; }
	UFUNCTION()
		float GetOnHookedVelocitySize(){ return OnHookedSpeed; }
	UFUNCTION()
		FVector GetOnHookedDirection(){ return OnHookedDirection; }
	UFUNCTION()
		FTransform GetOnHookedVehicleTransform(){return OnHookedVehicleTransfrom; }
	UFUNCTION()
		UGrappleSphereComponent* GetTargetComponent(){return TargetGrappableComponent; }
	UFUNCTION()
		bool IsGrappleInsideOfRange();
	
	UFUNCTION()
		void FireGrapplingHook();
	UFUNCTION()
		void RetractGrapplingHook();

	UFUNCTION()
		void ResetTemporalVariables();
	UFUNCTION()
		void OnGrappleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSensorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
	UFUNCTION()
		void EnterState(EGrappleStates NewState);
	UFUNCTION()
		void InActiveState();
	UFUNCTION()
		void TravelingState();
	UFUNCTION()
		void HookedState();
	UFUNCTION()
		void HookedEatableState();
	UFUNCTION()
		void ReturningState();

	UFUNCTION()
		void MoveTowardsGrapple(float LengthAtSpline);
	UFUNCTION()
		bool ValidGrappleState();

	//debug
public:
	UPROPERTY(EditAnywhere, Category = "Grapple|Debug")
		bool bDebugGrappleState = false;
};
