// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
#include "GrappleSphereComponent.h"
#include "Components/ActorComponent.h"

#include "Enums/Enums.h"

#include "PhysicsGrapplingComponent.generated.h"

//events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFoundHomingTarget, USceneComponent*, UGrappleSphereComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginHoming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLostHomingTarget);


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
	void HandleTargetHomingComp();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//my deseg ------------

	//events
public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FFoundHomingTarget FoundHomingTargetEvent;
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FBeginHoming BeginHomingEvent;
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FLostHomingTarget LostHomingTargetEvent;
	
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
		float LowestOnHookedSpeed = 4000.f;
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
		float GetOnHookedVelocitySize() const { return OnHookedSpeed; }
	UFUNCTION()
		FVector GetOnHookedDirection() const { return OnHookedDirection; }
	UFUNCTION()
		FTransform GetOnHookedVehicleTransform(){return OnHookedVehicleTransfrom; }
	UFUNCTION()
		UGrappleSphereComponent* GetTargetComponent() const {return TargetGrappableComponent; }
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
