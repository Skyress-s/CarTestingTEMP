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
	// UPROPERTY()
	// AActor* Owner = nullptr;
	// UPROPERTY(meta = (AllowPrivateAccess = "true"))
	// float SphereOverlapRadius = 100.f;
	// UPROPERTY()
	// class ACarPawn* CarPawn = nullptr;

public:
		
};
