// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Actor.h"
#include "PS/PS_PortalBase.h"

#include "PS_Weapon.generated.h"

UENUM( BlueprintType )
enum EPortalType : uint8
{
	Right,
	Left
};

class APSCharacter;

UCLASS()
class PS_API APS_Weapon : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default", meta=(AllowPrivateAccess=true))
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(AllowPrivateAccess=true))
	UInputMappingContext* PortalMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(AllowPrivateAccess=true))
	UInputAction* FirstPortalAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(AllowPrivateAccess=true))
	UInputAction* SecondPortalAction;

public:

	APS_Weapon();
	
	void AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName);
	
	

protected:
	
	virtual void BeginPlay() override;
	
	void Fire(EPortalType Type);
	
	void SpawnLineTrace(FVector& TargetPoint);
	
	void SpawnPortal(FHitResult HitResult, EPortalType Type);
	
	UPROPERTY(EditAnywhere, Category="Shoot", meta=(AllowPrivateAccess=true))
	float TraceDistance = 10000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(AllowPrivateAccess=true))
	USoundBase* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(AllowPrivateAccess=true))
	UAnimMontage* FireAnimation;
	
	UPROPERTY()
	APSCharacter* Character = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(AllowPrivateAccess=true))
	TSubclassOf<APS_PortalBase> Portal = nullptr;
	
	UPROPERTY()
	APS_PortalBase* LeftPortal = nullptr;
	
	UPROPERTY()
	APS_PortalBase* RightPortal = nullptr;
};
