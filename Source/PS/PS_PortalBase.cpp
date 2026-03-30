// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PortalBase.h"

#include "Camera/CameraComponent.h"
#include "Character/PSCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
	
	PassthroughBox = CreateDefaultSubobject<UBoxComponent>("PassthroughBox");
	PassthroughBox->SetupAttachment(SceneComponent);
	PassthroughBox->SetBoxExtent(FVector(100.0f, 80.0f, 100.0f));
	PassthroughBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PassthroughBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	PassthroughBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PassthroughBox->OnComponentBeginOverlap.AddDynamic(this, &APS_PortalBase::OnPassthroughBeginOverlap);
	PassthroughBox->OnComponentEndOverlap.AddDynamic(this, &APS_PortalBase::OnPassthroughEndOverlap);
	
	
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

void APS_PortalBase::UpdateCaptureTransform()
{
	if (!OtherPortal)
		return;
	
	APSCharacter* Character = Cast<APSCharacter>(UGameplayStatics::GetPlayerCharacter(OtherPortal, 0));
	if (!Character)
		return;
		
	FRotator LocalRotation = OtherPortal->GetActorTransform().InverseTransformRotation(Character->GetControlRotation().Quaternion()).Rotator();
	LocalRotation.Yaw += 180.0f;
	FRotator WorldRotation = GetActorTransform().TransformRotation(LocalRotation.Quaternion()).Rotator();
	
	float Distance = FVector::Dist(OtherPortal->GetActorLocation(), Character->GetActorLocation());
	float Area = 200.0f;
	float Alpha = FMath::Clamp(Area / Distance, 0.0f, 1.0f);
	
	FVector LocalPosition = OtherPortal->GetActorTransform().InverseTransformPosition(Character->GetFirstPersonCameraComponent()->GetComponentLocation());
	FVector WorldPosition = GetActorTransform().TransformPosition(LocalPosition);
	
	
	CaptureComponent->SetWorldRotation(WorldRotation);
	CaptureComponent->SetWorldLocation(FMath::Lerp(GetActorLocation(), WorldPosition, Alpha));
	
	/*UE_LOG(LogTemp, Warning, TEXT("Distance: %f | FOV: %f | Portal Position X=%f Y=%f Z=%f | Character Position X=%f Y=%f Z=%f"), 
		Distance, CaptureComponent->FOVAngle, 
		CaptureComponent->GetComponentTransform().GetLocation().X, CaptureComponent->GetComponentTransform().GetLocation().Y, CaptureComponent->GetComponentTransform().GetLocation().Z, 
		Character->GetFirstPersonCameraComponent()->GetComponentLocation().X, 
		Character->GetFirstPersonCameraComponent()->GetComponentLocation().Y, 
		Character->GetFirstPersonCameraComponent()->GetComponentLocation().Z);*/
	//CaptureComponent->SetWorldLocation(FMath::Lerp());
}

void APS_PortalBase::CheckTeleport()
{ 
	if (!OtherPortal)
		return;
	
	APSCharacter* Player = Cast<APSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
		return;

	FVector PortalForward = GetActorForwardVector();
	FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	
	float CurrentDot = FVector::DotProduct(PortalForward, ToPlayer);
	//UE_LOG(LogTemp, Warning, TEXT("Current Dot : %f"), CurrentDot);
	
	if (bPlayerWasInFront && CurrentDot < 0.f)
	{
		float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		//UE_LOG(LogTemp, Warning, TEXT("Distance : %f"), Distance);
		if (Distance < TeleportRadius)
		{
			UE_LOG(LogTemp, Warning, TEXT(" Player Location: X=%f Y=%f Z=%f | Player Rotation: Roll=%f Pitch=%f Yaw=%f"), 
				Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z,
				Player->GetActorRotation().Roll, Player->GetActorRotation().Pitch, Player->GetActorRotation().Yaw);
						
			Player->SetActorLocationAndRotation(
				OtherPortal->GetActorLocation(), Player->GetActorRotation());
			
			UE_LOG(LogTemp, Warning, TEXT(" New Player Location: X=%f Y=%f Z=%f | New Player Rotation: Roll=%f Pitch=%f Yaw=%f"), 
				Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z,
				Player->GetActorRotation().Roll, Player->GetActorRotation().Pitch, Player->GetActorRotation().Yaw);
			
			OtherPortal->bPlayerWasInFront = true;
		}
	}

	bPlayerWasInFront = CurrentDot >= 0.f;
	//UE_LOG(LogTemp, Log, TEXT("bPlayerWasInFront=%s"), bPlayerWasInFront ? TEXT("true") : TEXT("false"));
}

void APS_PortalBase::OnPassthroughBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	APSCharacter* Player = Cast<APSCharacter>(OtherActor);
	if (!Player || !WallComponent)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("WallComponent: %s"), WallComponent ? *WallComponent->GetName() : TEXT("None"));
	
	PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WallComponent->IgnoreActorWhenMoving(Player, true);
}

void APS_PortalBase::OnPassthroughEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APSCharacter* Player = Cast<APSCharacter>(OtherActor);
	if (!Player || !WallComponent)
		return;
	
	PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	WallComponent->IgnoreActorWhenMoving(Player, false);
}

void APS_PortalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (OtherPortal)
	{
		UpdateCaptureTransform();
		CheckTeleport();
	}
}

