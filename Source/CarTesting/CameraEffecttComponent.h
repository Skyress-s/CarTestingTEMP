// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraEffecttComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UCameraEffecttComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraEffecttComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//My deseg ---------------
private:
	UPROPERTY()
	class UCameraComponent* CameraCurrent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true", ToolTip = "Should NOT be longer than 3 seconds") , EditAnywhere, Category = "CameraEffect")
	class UCurveFloat* SpeedBoostCurve;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "CameraEffect")
	bool bPlaying = false;
	UPROPERTY()
	float Time = 0.f;
	UPROPERTY()
	float StartFOV = 90.f;
	
	
public:
	UFUNCTION()
	void SetCameraCurrent(class UCameraComponent* NewCamera){CameraCurrent = NewCamera; }
	UFUNCTION()
	void PlayCameraEffect();

};
