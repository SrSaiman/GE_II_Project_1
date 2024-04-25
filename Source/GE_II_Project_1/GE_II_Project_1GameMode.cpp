// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_Project_1GameMode.h"
#include "GE_II_Project_1Character.h"
#include "UObject/ConstructorHelpers.h"
#include <Blueprint/UserWidget.h>

AGE_II_Project_1GameMode::AGE_II_Project_1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> UI(TEXT("/Game/FirstPerson/Blueprints/UI_Cross"));
	if (UI.Class != NULL)
	{
		HUDWidgetClass = UI.Class;
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
