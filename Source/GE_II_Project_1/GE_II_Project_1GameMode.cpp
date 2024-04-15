// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_Project_1GameMode.h"
#include "GE_II_Project_1Character.h"
#include "UObject/ConstructorHelpers.h"

AGE_II_Project_1GameMode::AGE_II_Project_1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
