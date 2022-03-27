// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EVehicleState : uint8{
	Driving				UMETA(DisplayName = "EDriving"),
	Grappling			UMETA(DisplayName = "EGrappling"),
	AirBorne			UMETA(DisplayName = "EAirBorne"),
	Dashing				UMETA(DisplayName = "EDashing")
};

UENUM(BlueprintType)
enum class EGrappleStates : uint8{
	InActive				UMETA(DisplayName = "EInActive"),
	Traveling				UMETA(DisplayName = "ETraveling"),
	Hooked					UMETA(DisplayName = "EHooked"),
	HookedEatable			UMETA(DisplayName = "EHookedEatable"),
	Returning				UMETA(DisplayName = "EReturning")
	};