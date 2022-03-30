// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/Enums.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"


UCLASS()
class CARTESTING_API ACarPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RotateSphereCompToLocalUpVector();
	void ApplyGravity();
	void TiltCarMesh(FVector AsymVector);
	void HandleAsymFriction(FVector& AsymVector);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// my deseg
public:
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class USphereComponent* SphereComp{ nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UStaticMeshComponent* CarMesh{ nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class USpringArmComponent* CameraBoom{ nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UCameraComponent* MainCamera{ nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UArrowComponent* ArrowRayCastStart{ nullptr };
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UBoostComponent* BoostComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class USphereComponent* GrappleHookSphereComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UStaticMeshComponent* GrappleHookMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UStaticMeshComponent* GrappleSensor = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UPhysicsGrapplingComponent* PhysicsGrappleComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UCameraEffecttComponent* CameraEffectComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
		class UNeckComponent* NeckComponent = nullptr;

	// spline neck
	UPROPERTY(EditDefaultsOnly, Category = "Car|Neck")
		class USplineComponent* NeckSpline = nullptr;
	
	//spline gravity
	UPROPERTY(EditAnywhere, Category = "Car|spline")
		class AGravitySplineActor* GravitySplineActive = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
		float HoverHeight{50};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
		float HoverCutOffHeight{1.5f*HoverHeight};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
		float HoverDampingFactor{4};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
		float HoverForceReduction{20};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Car|Hover")
		float GravityForce{2720};
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Hover")
		float GravityMod = 1.f;
	
	const float BaseGravMod = 1.f;
	
	UFUNCTION()
	void OnHitt(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	FVector LocalUpVector = FVector(1.f,1.f,1.f);
	UFUNCTION()
	void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex);

	

private:

	//movement
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float AccelerationForce = 90000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float DeaccelerationForce = 400000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxSpeed = 7500.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float TurnSpeed = 50.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxGroundAngle = 75.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxCar_SplineAngle = 85.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxCar_SplineAngleCorrectionSpeed = 100.f;

	//Camera variables
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Camera")
	float CameraLookSpeed = 70.f; //gets set at begin play
	UPROPERTY()
	float StartCameraBoomLength = 0.f; //gets set at begin play
	UPROPERTY() 
	FVector2D OnStartCameraLag = FVector2D::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true", ToolTip = "X is CameraLag,  Y is CameraRotationLag"), EditDefaultsOnly, Category = "Car|Camera")
	FVector2D GrapplingCameraLag = FVector2D::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|Camera")
	float MaxYawLookAngle = 90.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|Camera")
	float MaxPichLookAngle = 45.f;

	//Deathzone
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|Deathzone")
		float MaxOutOfBoundsDistance = 10000.f;
	UPROPERTY()
		FVector StartPlayerLocation = FVector::ZeroVector;
	
	//state machince
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|State")
	EVehicleState CurrentVehicleState = EVehicleState::AirBorne;

	bool bEnterState = false;
	UFUNCTION()
	void EnterState(EVehicleState NewState);
	
	
	UFUNCTION()
	void StateDriving();
	
	UFUNCTION()
	void StateGrappling();
	UPROPERTY(EditAnywhere, Category = "Car|State")
	float FinishGrappleDistance = 100.f;
	
	UFUNCTION()
	void StateAirBorne();
	
	UFUNCTION()
	void StateDashing();
	
	UFUNCTION()
	void ToggleGrappleHook();
	
	//TODO orgenize these :)
	//other funcs
	
	FVector CalcAsymVector();
	float CaltAsymForce();
	void MoveXAxis(float Value);
	void MoveYAxis(float Value);
	void LookXAxis(float Value);
	void LookYAxis(float Value);
	
	UFUNCTION(BlueprintCallable)
	static float SignedAngleAxis(FVector v1, FVector v2, FVector axis);
	float UnsignedAngle(FVector v1, FVector v2);
	bool IsGrounded();
	float DistanceToGround();
	FVector VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target);
	FHitResult ShootRayFromCenterOfScreen();
	void SetUpVectorAsSplineUpAxis();
	bool IsMovingForward();

	float GetSplineCarForwardAngle();
	void HandleMaxTurnWithSpline();

	
	UFUNCTION()
	void SpeedHandleCameraBoomEffect(bool bSoft);
	
	UFUNCTION()
	bool IsOutOfBounds();
	

public:
	UFUNCTION()
	void HandleBoost();
	bool IsUnderMaxSpeed(bool bBuffer);


};
