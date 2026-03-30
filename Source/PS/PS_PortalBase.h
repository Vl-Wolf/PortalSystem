// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "PS_PortalBase.generated.h"

UCLASS()
class PS_API APS_PortalBase : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	USceneComponent* SceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal", meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* PortalMesh;
		
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	UDecalComponent* DecalComponent;*/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	USceneCaptureComponent2D* CaptureComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal", meta=(AllowPrivateAccess=true))
	UBoxComponent* PassthroughBox;

public:

	APS_PortalBase();
	
	void LinkToPortal(APS_PortalBase* Other);
	
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
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USceneCaptureComponent2D* GetOtherSceneCapture() const
	{
		return OtherPortal->CaptureComponent;
	}
	
	/*UFUNCTION(BlueprintCallable, BlueprintPure)
	UDecalComponent* GetOtherDecalComponent() const
	{
		return DecalComponent;
	}*/
	
	void SetWallComponent(UPrimitiveComponent* Wall)
	{
		WallComponent = Wall;
	}

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FColor PortalColor = FColor::Green;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	APS_PortalBase* OtherPortal = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	UTextureRenderTarget2D* RenderTarget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	FIntPoint RenderTargetResolution = FIntPoint(1024, 1024);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Render", meta=(AllowPrivateAccess=true))
	UMaterialInterface* PortalBaseMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Render|Debug", meta=(AllowPrivateAccess=true))
	bool bIsDebug = false;
		
	void UpdateCaptureTransform();
	
	void CheckTeleport();
	
	bool bPlayerWasInFront = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", meta=(AllowPrivateAccess=true))
	float TeleportRadius = 200.0f;
	
	UPROPERTY()
	UPrimitiveComponent* WallComponent = nullptr;
	
	UFUNCTION()
	void OnPassthroughBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPassthroughEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:

	virtual void Tick(float DeltaTime) override;
};
