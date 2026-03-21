// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Weapon.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "PS/Character/PSCharacter.h"


// Sets default values
APS_Weapon::APS_Weapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APS_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PortalMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FirstPortalAction, ETriggerEvent::Triggered, this, &APS_Weapon::Fire);
			EnhancedInputComponent->BindAction(SecondPortalAction, ETriggerEvent::Triggered, this, &APS_Weapon::Fire);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}
}

void APS_Weapon::Fire()
{
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	if (FireAnimation != nullptr)
	{
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

void APS_Weapon::SpawnLineTrace(FVector& TargetPoint)
{
	FTransform NozzleTransform = SkeletalMesh->GetSocketTransform(TEXT("Muzzle"), RTS_World);
	FVector NozzleLocation = NozzleTransform.GetLocation();
	FVector Direction = (TargetPoint - NozzleLocation).GetSafeNormal();
	
	DrawDebugLine(GetWorld(), NozzleLocation, TargetPoint, FColor::Red, false, 1.0f, 0, 1.5f);
}

void APS_Weapon::AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName)
{
	if (!CharacterMesh)
		return;
	
	Character = Cast<APSCharacter>(GetInstigator());
	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, SocketName);
}

