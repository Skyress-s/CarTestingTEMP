// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "SingleRunCameraModifier.generated.h"

/**
 * This verison will only run ONCE when added to the Player camera Manager, when the Alpha Inn phase is done, it will disable itself
 */
UCLASS()
class CARTESTING_API USingleRunCameraModifier : public UCameraModifier
{
	GENERATED_BODY()
	//my deseg ----------

	
public:
	virtual void EnableModifier() override;
private:
	
	
};
