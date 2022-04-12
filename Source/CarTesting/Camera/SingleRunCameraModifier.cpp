// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleRunCameraModifier.h"

void USingleRunCameraModifier::EnableModifier()
{
	// simply makes sure it disables it self after the in phase is finished
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]
	{
		UE_LOG(LogTemp, Warning, TEXT("SingleRunModifierDisablesItself"))
		this->DisableModifier(false);
		
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AlphaInTime, false);

	Super::EnableModifier();

}
