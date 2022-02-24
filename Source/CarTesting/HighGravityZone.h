// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HighGravityZone.generated.h"

UCLASS()
class CARTESTING_API AHighGravityZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHighGravityZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// my deseg
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Gravity")
		float GravityModifier = 3.f;
public:
	UPROPERTY(EditAnywhere, Category = "Gravity")
		class UBoxComponent* BoxComp = nullptr;
	UFUNCTION()
		float GetGravityModifier() { return GravityModifier; }
};
