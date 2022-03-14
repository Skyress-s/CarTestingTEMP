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
	// UPROPERTY(EditDefaultsOnly, Category = "Car")
	// 	class UGrappleComponent* GrappleComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UStaticMeshComponent* GrappleHookMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UStaticMeshComponent* GrappleSensor = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UPhysicsGrapplingComponent* PhysicsGrappleComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UCameraEffecttComponent* CameraEffectComponent = nullptr;
	
	//spline gravity
	UPROPERTY(EditAnywhere, Category = "Car|spline")
		class AGravitySplineActor* GravitySplineActive = nullptr;
		 

	           
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
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float Acceleration = 70000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxSpeed = 3500.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float GravityMod = 1.f;
	const float BaseGravMod = 1.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float MaxAngle = 75.f;
	UPROPERTY()
	FVector StartPlayerLocation = FVector::ZeroVector;

	UPROPERTY()
	FVector2D OnStartCameraLag = FVector2D::ZeroVector;
	UPROPERTY(meta = (AllowPrivateAccess = "true", ToolTip = "X is CameraLag,  Y is CameraRotationLag"), EditDefaultsOnly, Category = "Car|Movment")
	FVector2D GrapplingCameraLag = FVector2D::ZeroVector;

	UPROPERTY()
	float StartCameraBoomLength = 0.f;
	
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
	

	FVector CalcAsymVector();
	float CaltAsymForce();
	void MoveXAxis(float Value);
	void MoveYAxis(float Value);
	void LookXAxis(float Value);
	void LookYAxis(float Value);
	UFUNCTION()
	void HandleBoost();
	UFUNCTION(BlueprintCallable)
	static float SignedAngleAxis(FVector v1, FVector v2, FVector axis);
	float UnsignedAngle(FVector v1, FVector v2);
	bool IsGrounded();
	FVector VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target);
	FHitResult ShootRayFromCenterOfScreen();
	void SetUpVectorAsSplineUpAxis();

	
	UFUNCTION()
	void SpeedHandleCameraBoomEffect(bool bSoft);
	
	UFUNCTION()
	bool IsOutOfBounds();
	

public:
	bool IsUnderMaxSpeed(bool bBuffer);


};
