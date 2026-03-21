// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PSWeaponComponent.generated.h"

class APSCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PS_API UPSWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class APSProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* PortalMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FirstPortalAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* SecondPortalAction;
	
	UPSWeaponComponent();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(APSCharacter* TargetCharacter);
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();
	
	void SpawnLineTrace(FVector& TargetPoint);

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	APSCharacter* Character;
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay, meta=(AllowPrivateAccess = "true"))
	float TraceDistance = 10000.0f;
};
