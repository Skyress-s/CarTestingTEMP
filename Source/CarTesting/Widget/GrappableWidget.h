// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GrappableWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARTESTING_API UGrappableWidget : public UUserWidget
{
	GENERATED_BODY()
	//my deseg ----------
public:
	UPROPERTY(Transient, meta = ( BindWidgetAnim ))
		UWidgetAnimation* WidgetAnim;
	
	UFUNCTION(BlueprintCallable)
		void PlayCloseAnimationCpp();
		

private:
	
};
