// Copyright 2020 Northview High School Game Development Club


#include "CP_HUD_MainMenu.h"
#include "CP_MainMenu.h"
#include "GameFramework/PlayerController.h"

// Called when game begins
void ACP_HUD_MainMenu::BeginPlay()
{
	Super::BeginPlay();

	/*Creates main menu widget and puts it on screen*/
	if (MainMenuWidgetClass)
	{
		UCP_MainMenu* MainMenuWidget = CreateWidget<UCP_MainMenu>(GetWorld(), MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();

			/* https://blog.gamedev.tv/creating-unreal-engine-ui-with-umg-and-c/ */
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			GetOwningPlayerController()->SetInputMode(InputMode);
		}
	}
}