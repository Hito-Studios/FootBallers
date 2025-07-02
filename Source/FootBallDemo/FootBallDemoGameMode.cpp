// Copyright Epic Games, Inc. All Rights Reserved.

#include "FootBallDemoGameMode.h"
#include "FootBallDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFootBallDemoGameMode::AFootBallDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
