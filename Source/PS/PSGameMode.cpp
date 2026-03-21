// Copyright Epic Games, Inc. All Rights Reserved.

#include "PSGameMode.h"
#include "PSCharacter.h"
#include "UObject/ConstructorHelpers.h"

APSGameMode::APSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
