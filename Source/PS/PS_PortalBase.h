// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PS_PortalBase.generated.h"

UCLASS()
class PS_API APS_PortalBase : public AActor
{
	GENERATED_BODY()

public:

	APS_PortalBase();
	
	void SetPortalColor(FColor Color)
	{
		PortalColor = Color;
	}
	
	void SetOtherPortal(APS_PortalBase* Portal)
	{
		OtherPortal = Portal;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FColor GetPortalColor() const
	{
		return PortalColor;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	APS_PortalBase* GetOtherPortal() const
	{
		return OtherPortal;
	}

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FColor PortalColor = FColor::Green;
	
	UPROPERTY()
	APS_PortalBase* OtherPortal = nullptr;

public:

	virtual void Tick(float DeltaTime) override;
};
