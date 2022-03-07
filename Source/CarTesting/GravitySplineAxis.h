// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GravitySplineAxis.generated.h"

UENUM(BlueprintType)
enum class EGravitySplineAxis : uint8{
	Axis_X			UMETA(DisplayName = "AxisX"),
	Axis_Y			UMETA(DisplayName = "AxisY"),
	Axis_Z			UMETA(DisplayName = "AxisZ"),
	Axis_W			UMETA(DisplayName = "AxisW")
};


