// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
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
	float OnHookedVelocity = 0.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple")
	float FireGrappleSpeed = 10000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Grapple")
	float GrappleRotationSpeed = 500.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
	FVector OnHookedDirection = FVector::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
	FVector TravelingDirection = FVector::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
	FVector HomingTargetLocation = FVector::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
	bool bHoming = false;
	UPROPERTY()
	float MoveToTargetModifier = 1.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple")
	float MoveToTargetAcceleration = 1.3f;
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple|OnHooked")
	float LowestOnHookedSpeed = 2000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere ,Category = "Grapple|OnHooked")
	float HighestOnHookedSpeed = 9000.f;
	
public:
	UFUNCTION()
	EGrappleStates GetCurrentGrappleState(){return CurrentGrappleState; }
	UFUNCTION()
	float GetOnHookedVelocitySize(){ return OnHookedVelocity; }
	UFUNCTION()
	FVector GetOnHookedDirection(){ return OnHookedDirection; }
	UFUNCTION()
	FVector GetTravelingDirection(){ return TravelingDirection; }
	
	UFUNCTION()
	void FireGrapplingHook();
	UFUNCTION()
	void RetractGrapplingHook();

	UPROPERTY()
	class UGrappleSphereComponent* TempGrappleSphereComponent = nullptr;
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
	void ReturningState();

	UFUNCTION()
	void MoveTowardsGrapple();
	UFUNCTION()
	bool ValidGrappleState();
		
};
