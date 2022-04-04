// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarTesting/Widget/GrappableWidget.h"
#include "Components/ActorComponent.h"
#include "GrappableWidgetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARTESTING_API UGrappableWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappableWidgetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//my deseg



	
	UFUNCTION()
		void PlayAnimation(USceneComponent* NewSceneComponent);
	
private:
	UFUNCTION()
		void SetupWidget();
	
	UPROPERTY()
		class UWidgetComponent* WidgetComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Widget")
		TSubclassOf<UGrappableWidget> GrappableWidget = nullptr;

	
	UPROPERTY()
		class ACarPawn* CarPawn = nullptr;
	UPROPERTY()
		USceneComponent* CurrentAttachComponent = nullptr;
};
