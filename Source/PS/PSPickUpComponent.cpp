// Copyright Epic Games, Inc. All Rights Reserved.

#include "PSPickUpComponent.h"

UPSPickUpComponent::UPSPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UPSPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UPSPickUpComponent::OnSphereBeginOverlap);
}

void UPSPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	APSCharacter* Character = Cast<APSCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
