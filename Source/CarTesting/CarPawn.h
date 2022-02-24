// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	
	//spline gravity
	UPROPERTY(EditAnywhere, Category = "Car|spline")
		class AGravitySplineActor* GravitySplineActive = nullptr;

	           
	UFUNCTION()
	void OnHitt(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	FVector LocalUpVector = FVector(1.f,1.f,1.f);
	//UFUNCTION()
	/*void OnBeginOverLap(UPrimitiveComponent* OnComponentBeginOverlap, UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/



private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float Acceleration = 70000.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
		float MaxSpeed = 3500.f;

	FVector CalcAsymVector();
	float CaltAsymForce();
	void MoveXAxis(float Value);
	void MoveYAxis(float Value);
	float SignedAngleAxis(FVector v1, FVector v2, FVector axis);
	float UnsignedAngle(FVector v1, FVector v2);
	bool IsGrounded();
	FVector VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target);

public:
	bool IsUnderMaxSpeed(bool bBuffer);


};
