// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PortalBase.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextRenderComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"

APS_PortalBase::APS_PortalBase()
{

	PrimaryActorTick.bCanEverTick = true;
	
	/*DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	SetRootComponent(DecalComponent);*/
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>("PortalMesh");
	PortalMesh->SetupAttachment(SceneComponent);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("CaptureComponent");
	CaptureComponent->SetupAttachment(SceneComponent);
	CaptureComponent->bCaptureEveryFrame = true; // Test
	CaptureComponent->bCaptureOnMovement = true;
	
	TeleportTrigger = CreateDefaultSubobject<UBoxComponent>("TeleportTrigger");
	TeleportTrigger->SetupAttachment(SceneComponent);
	//TeleportTrigger->OnComponentBeginOverlap.AddDynamic(this, &APS_PortalBase::OnTriggerBeginOverlap);
}

void APS_PortalBase::LinkToPortal(APS_PortalBase* Other)
{
	if (!Other || Other == this)
		return;
	
	OtherPortal = Other;
	
	if (Other->RenderTarget && PortalBaseMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(PortalBaseMaterial, this);
		DynamicMaterial->SetTextureParameterValue(TEXT("PortalTexture"), Other->RenderTarget);
		DynamicMaterial->SetVectorParameterValue(TEXT("PortalColor"), PortalColor);
		PortalMesh->SetMaterial(0, DynamicMaterial);
	}
}


void APS_PortalBase::BeginPlay()
{
	Super::BeginPlay();
	
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(RenderTargetResolution.X, RenderTargetResolution.Y);
	RenderTarget->UpdateResourceImmediate(true);
	
	CaptureComponent->TextureTarget = RenderTarget;
}

/*void APS_PortalBase::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherPortal)
		return;
	
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (!Player)
		return;
	
	FVector LocalPosition = GetActorTransform().InverseTransformPosition(Player->GetActorLocation());
	LocalPosition.X = -LocalPosition.X;
	
	FVector NewLocation = OtherPortal->GetActorTransform().TransformPosition(LocalPosition);
	
	FRotator LocalRot = GetActorTransform().InverseTransformRotation(Player->GetActorRotation().Quaternion()).Rotator();
	LocalRot.Yaw += 180.f;
	FRotator NewRotation = OtherPortal->GetActorTransform().TransformRotation(LocalRot.Quaternion()).Rotator();

	Player->TeleportTo(NewLocation, NewRotation);
}*/

void APS_PortalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

