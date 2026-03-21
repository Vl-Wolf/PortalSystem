// Copyright Epic Games, Inc. All Rights Reserved.


#include "PSWeaponComponent.h"
#include "PSCharacter.h"
#include "PSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UPSWeaponComponent::UPSWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UPSWeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	/*if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<APSProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}*/
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	
	FVector TargetPoint = FVector::ZeroVector;
	
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
		
		const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * TraceDistance;
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(Character);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, CollisionQueryParams))
		{
			TargetPoint = HitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), TargetPoint, 8.0f, 8, FColor::Red, false, 1.0f, 0, 1.5f);
		}
		else
		{
			TargetPoint = TraceEnd;
			DrawDebugSphere(GetWorld(), TargetPoint, 8.0f, 8, FColor::Red, false, 1.0f, 0, 1.5f);
		}
		
		SpawnLineTrace(TargetPoint);
	}
	
}

void UPSWeaponComponent::SpawnLineTrace(FVector& TargetPoint)
{
	FTransform NozzleTransform = GetSocketTransform(TEXT("Muzzle"), RTS_World);
	FVector NozzleLocation = NozzleTransform.GetLocation();
	FVector Direction = (TargetPoint - NozzleLocation).GetSafeNormal();
	
	DrawDebugLine(GetWorld(), NozzleLocation, TargetPoint, FColor::Red, false, 1.0f, 0, 1.5f);
}

bool UPSWeaponComponent::AttachWeapon(APSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UPSWeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(PortalMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FirstPortalAction, ETriggerEvent::Triggered, this, &UPSWeaponComponent::Fire);
			EnhancedInputComponent->BindAction(SecondPortalAction, ETriggerEvent::Triggered, this, &UPSWeaponComponent::Fire);
		}
		else
			UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}

	return true;
}

void UPSWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(PortalMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}