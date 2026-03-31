// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PortalBase.h"

#include "Camera/CameraComponent.h"
#include "Character/PSCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;
	
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
	FVector2D Resolution;
	GetWorld()->GetGameViewport()->GetViewportSize(Resolution);
	RenderTarget->InitAutoFormat(Resolution.X, Resolution.Y);
	RenderTarget->UpdateResourceImmediate(true);
	
	CaptureComponent->TextureTarget = RenderTarget;
	
}

void APS_PortalBase::UpdateCaptureTransform()
{
	if (!OtherPortal)
		return;
	
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	
	//APSCharacter* Character = Cast<APSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!CameraManager)
		return;
		
	FRotator BaseRotation = CameraManager->GetTransformComponent()->GetComponentRotation(); 
	FVector X = FVector::ZeroVector; 
	FVector Y = FVector::ZeroVector; 
	FVector Z = FVector::ZeroVector;
	
	UKismetMathLibrary::BreakRotIntoAxes(BaseRotation, X, Y, Z);
	
	FVector LocalForward = UKismetMathLibrary::MirrorVectorByNormal(
		UKismetMathLibrary::MirrorVectorByNormal(
			OtherPortal->GetActorTransform().InverseTransformVectorNoScale(X), 
		FVector(1.0f, 0.0, 0.0f)),
		FVector(0.0f, 1.0f, 0.0f));
	
	FVector LocalRight = UKismetMathLibrary::MirrorVectorByNormal(
		UKismetMathLibrary::MirrorVectorByNormal(
			OtherPortal->GetActorTransform().InverseTransformVectorNoScale(Y), 
		FVector(1.0f, 0.0, 0.0f)),
		FVector(0.0f, 1.0f, 0.0f));
	
	FVector LocalUp = UKismetMathLibrary::MirrorVectorByNormal(
		UKismetMathLibrary::MirrorVectorByNormal(
			OtherPortal->GetActorTransform().InverseTransformVectorNoScale(Z), 
		FVector(1.0f, 0.0, 0.0f)),
		FVector(0.0f, 1.0f, 0.0f));
	
	FVector WorldForward = GetTransform().TransformVectorNoScale(LocalForward);
	FVector WorldRight = GetTransform().TransformVectorNoScale(LocalRight);
	FVector WorldUp = GetTransform().TransformVectorNoScale(LocalUp);
	
	FRotator WorldRotation = UKismetMathLibrary::MakeRotationFromAxes(WorldForward, WorldRight, WorldUp);
		
	CaptureComponent->SetWorldRotation(WorldRotation);
	
	FVector LocalPosition = OtherPortal->GetActorTransform().InverseTransformPosition(CameraManager->GetCameraLocation());
	FVector WorldPosition = GetActorTransform().TransformPosition(LocalPosition);
	
	CaptureComponent->SetWorldLocation(WorldPosition);
}

void APS_PortalBase::CheckTeleport()
{ 
	if (!OtherPortal)
		return;
	
	APSCharacter* Player = Cast<APSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
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
			/*UE_LOG(LogTemp, Warning, TEXT(" Player Location: X=%f Y=%f Z=%f | Player Rotation: Roll=%f Pitch=%f Yaw=%f"), 
				Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z,
				Player->GetActorRotation().Roll, Player->GetActorRotation().Pitch, Player->GetActorRotation().Yaw);*/
			
			FRotator PortalRotationDiff = (OtherPortal->GetActorRotation() - GetActorRotation()).GetNormalized();
			PortalRotationDiff.Yaw += 180.0f;
			
			FRotator NewRotation = Player->GetActorRotation() + PortalRotationDiff;
			NewRotation.Normalize();			
						
			Player->SetActorLocationAndRotation(
				OtherPortal->GetActorLocation(), NewRotation);
			
			if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
			{
				FRotator ControlRotation = PC->GetControlRotation() + PortalRotationDiff;
				ControlRotation.Normalize();
				PC->SetControlRotation(ControlRotation);
			}
			
			/*UE_LOG(LogTemp, Warning, TEXT(" New Player Location: X=%f Y=%f Z=%f | New Player Rotation: Roll=%f Pitch=%f Yaw=%f"), 
				Player->GetActorLocation().X, Player->GetActorLocation().Y, Player->GetActorLocation().Z,
				Player->GetActorRotation().Roll, Player->GetActorRotation().Pitch, Player->GetActorRotation().Yaw);*/
			
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
	if (!Player || !WallComponent || !OtherPortal)
		return;
	
	UE_LOG(LogTemp, Log, TEXT("WallComponent: %s"), WallComponent ? *WallComponent->GetName() : TEXT("None"));
	
	PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WallComponent->IgnoreActorWhenMoving(Player, true);
	
	OtherPortal->PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	OtherPortal->WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	OtherPortal->WallComponent->IgnoreActorWhenMoving(Player, true);
}

void APS_PortalBase::OnPassthroughEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APSCharacter* Player = Cast<APSCharacter>(OtherActor);
	if (!Player || !WallComponent || !OtherPortal)
		return;
	
	PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	WallComponent->IgnoreActorWhenMoving(Player, false);
	
	OtherPortal->PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OtherPortal->WallComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OtherPortal->WallComponent->IgnoreActorWhenMoving(Player, false);
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

