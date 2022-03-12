// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoostComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UBoostComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoostComponent();

	//public Sphere component that

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Boost|Movment")
	UPrimitiveComponent* PhysComp = nullptr;
	UPROPERTY(EditAnywhere, Category = "Boost")
	float BoostDuration = 1.f;
	UPROPERTY(EditAnywhere, Category = "Boost")
	class UCurveFloat* BoostCurve = nullptr;
	UPROPERTY()
	bool bBoosting = false;
	UPROPERTY()
	float CurrentBoostTime = 0.f;
	
	UPROPERTY()
	FTimerHandle BoostDurationTimerHandle;

	UFUNCTION()
	void Disable_bBoosting() {
		bBoosting = false;
		CurrentBoostTime = 0.f;
	}

	UFUNCTION()
		void Boost();

private:
	class ACarPawn* CarPawn = nullptr;

};
