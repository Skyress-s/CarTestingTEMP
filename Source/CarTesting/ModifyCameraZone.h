// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ModifyCameraZone.generated.h"

UCLASS()
class CARTESTING_API AModifyCameraZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AModifyCameraZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// my deseg
	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxComponent = nullptr;

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Zone")
		float CameraBoomLength = 0.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Zone")
		bool bAddative = true;
	//funcs
	UFUNCTION()
		void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor*
			OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
