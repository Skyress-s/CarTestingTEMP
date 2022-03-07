// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleTarget.generated.h"

UCLASS()
class CARTESTING_API AGrappleTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrappleTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
// my deseg
public:
	// UPROPERTY(EditAnywhere, Category = "GrappleTarget")
	// class UBillboardComponent* BillboardComponent = nullptr;
	// UPROPERTY(EditAnywhere, Category = "GrappleTarget")
	// class UWidgetComponent* Widgett = nullptr;
	// UPROPERTY(EditAnywhere, Category = "GrappleTarget")
	// class USphereComponent* SphereComponent = nullptr;
};
