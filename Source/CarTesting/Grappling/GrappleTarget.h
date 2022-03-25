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
private:
	/*UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	class UBillboardComponent* BillboardComponent = nullptr;*/
	/*UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	class UWidgetComponent* Widget = nullptr;*/
	// UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	// class USphereComponent* SphereComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	class UGrappleSphereComponent* GrappleSphereComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	class USkeletalMeshComponent* MainMesh = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
	class UBoxComponent* Trigger = nullptr;
	
	
	
public:
	void SetVisbility(bool bVisible);
	UFUNCTION()
	void OnReachedTarget(float AddSpeedAmount);
	UFUNCTION()
	void OnGrappleTarget(FTransform SphereCompTransfrom);
	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
