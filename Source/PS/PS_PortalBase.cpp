// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_PortalBase.h"

APS_PortalBase::APS_PortalBase()
{

	PrimaryActorTick.bCanEverTick = true;
}


void APS_PortalBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void APS_PortalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

