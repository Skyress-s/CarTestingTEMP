// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicEatableActor.generated.h"

UCLASS()
class CARTESTING_API ABasicEatableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicEatableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
// my deseg ------------
	UPROPERTY(EditAnywhere, Category = "Eatable")
		class UStaticMeshComponent* MainMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Eatable")
		class UGrappleSphereComponent* GrappleSphereComponent = nullptr;

	UFUNCTION()
		void OnReached(float AddSpeedAmount);
};
