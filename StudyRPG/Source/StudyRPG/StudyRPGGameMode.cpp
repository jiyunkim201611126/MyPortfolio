// Copyright Epic Games, Inc. All Rights Reserved.

#include "StudyRPGGameMode.h"
#include "StudyRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStudyRPGGameMode::AStudyRPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
