// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
// my deseg
private:
	UPROPERTY()
	AActor* Owner = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	float SphereOverlapRadius = 400.f;
	UPROPERTY()
	class ACarPawn* CarPawn = nullptr;
public:
	UPROPERTY()
	TArray<class AGrappleTarget*> GrappleTargetsLastFrame;
private:
	
	UPROPERTY()
	AActor* Target = nullptr;
	UPROPERTY()
	float GrappleSpeed = 50.f;
	UPROPERTY()
	FVector DirectionAtStart = FVector::ZeroVector;
public:
	void SetTarget(AActor* NewTarget);
	AActor* GetTarget() const;
	
	void SetGrappleSpeed(float NewSpeed) { NewSpeed > 529.f ? GrappleSpeed = NewSpeed : GrappleSpeed = 529.f; }
	float GetGrappleSpeed(){return GrappleSpeed; }

	void SetDirectionAtStart(FVector NewDirection) {DirectionAtStart = NewDirection; }
	FVector GetDirectionAtStart(){ return DirectionAtStart; }
	
	void MoveTowardsTarget();
	float DistanceToTargetSqr();
	
	

public:
		
};
