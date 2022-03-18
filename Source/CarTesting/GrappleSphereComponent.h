// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GrappleSphereComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrappleEvent, float, AddSpeedAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedEvent, float, AddSpeedAmount);

/**
 * 
 */
UCLASS()
class CARTESTING_API UGrappleSphereComponent : public USphereComponent
{
	GENERATED_BODY()
	UGrappleSphereComponent();
public:
	//My deseg
	//these are called by the Physics grapple component
	UPROPERTY(BlueprintAssignable , Category = "Event")
	FOnGrappleEvent OnGrappleHitEvent;
	UPROPERTY(BlueprintAssignable ,Category = "Event")
	FOnReachedEvent OnReachedEvent;

	
	
	UFUNCTION()
	void OnGrapple();
	UFUNCTION()
	void OnReached();

	UFUNCTION()
	float GetAddSpeed() const {return AddSpeed; }
private:
	UPROPERTY()
	float AddSpeed = 550.f;
	
};