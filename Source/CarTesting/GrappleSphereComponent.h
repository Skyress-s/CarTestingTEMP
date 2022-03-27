// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GrappleSphereComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrappleEvent, FTransform, SphereCompTransfrom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedEvent, float, AddSpeedAmount);

/**
 * 
 */
UCLASS()
class CARTESTING_API UGrappleSphereComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UGrappleSphereComponent();
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
	UFUNCTION()
		bool IsEatable() const {return bEatable; }

	UFUNCTION()
		void SetIsEatable(bool bEnable) {bEatable = bEnable; }
protected:
	UPROPERTY()
		float AddSpeed = 550.f;
	UPROPERTY(EditAnywhere, Category = "Eatable")
		bool bEatable = false;
	
};
