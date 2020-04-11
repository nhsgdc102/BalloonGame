// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CP_HUD_MainMenu.generated.h"

/**
 * 
 */
class UCP_MainMenu;

UCLASS()
class BALLOONGAME_API ACP_HUD_MainMenu : public AHUD
{
	GENERATED_BODY()

protected:
	// Called when game begins
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UCP_MainMenu> MainMenuWidgetClass;
};
