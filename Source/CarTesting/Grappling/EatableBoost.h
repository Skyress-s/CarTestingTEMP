// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EatableBoost.generated.h"

UCLASS()
class CARTESTING_API AEatableBoost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEatableBoost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
//my deseg --------
public:
	UPROPERTY(EditAnywhere, Category = "Eatable")
		class UStaticMeshComponent* StaticMeshComponent = nullptr;
};
