// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
//For get world delta seconds
#include "Engine/World.h"
//For viewport client
#include "Engine/GameViewportClient.h"

#include "BalloonPop_HUD.generated.h"

/**
 *
 */
UCLASS()
class BALLOONGAME_API ABalloonPop_HUD : public AHUD
{
	GENERATED_BODY()

public:
	//Sets default values
	ABalloonPop_HUD();

	//Override of DrawHUD() in base HUD class
	virtual void DrawHUD();

	UFUNCTION(BlueprintCallable)
		void fadeIn();

	UFUNCTION(BlueprintCallable)
		void fadeOut();

	//Fading
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadingBetweenLevels")
		float fadeDuration;

private:
	float fadeRate;
	float fadeOpacity;
	float fadeDirection; //-1 for fade out, +1 for fade in
	bool bIsFading;

public:
	FVector2D screenSize;
};
