// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleSphereComponent.h"

UGrappleSphereComponent::UGrappleSphereComponent()
{
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
}

void UGrappleSphereComponent::OnGrapple()
{
	OnGrappleHitEvent.Broadcast(GetComponentTransform());
	UE_LOG(LogTemp, Warning, TEXT("On Grapple(From GrapleSphereComponent!)"))
}

void UGrappleSphereComponent::OnReached()
{
	OnReachedEvent.Broadcast(AddSpeed);
	UE_LOG(LogTemp, Warning, TEXT("On Reached (From GrapleSphereComponent!)"))
}
