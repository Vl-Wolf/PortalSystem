// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Actor.h"

#include "PS_Weapon.generated.h"

class APSCharacter;

UCLASS()
class PS_API APS_Weapon : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default", meta=(AllowPrivateAccess=true))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default", meta=(AllowPrivateAccess=true))
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* PortalMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess = "true"))
	UInputAction* FirstPortalAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta=(AllowPrivateAccess = "true"))
	UInputAction* SecondPortalAction;

public:

	APS_Weapon();
	
	void AttachToCharacter(USkeletalMeshComponent* CharacterMesh, FName SocketName);

protected:
	
	virtual void BeginPlay() override;
	
	void Fire();
	
	void SpawnLineTrace(FVector& TargetPoint);
	
	
	
	UPROPERTY(EditAnywhere, Category="Default", meta=(AllowPrivateAccess=true))
	float TraceDistance = 10000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(AllowPrivateAccess = "true"))
	USoundBase* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* FireAnimation;
	
	UPROPERTY()
	APSCharacter* Character = nullptr;
};
