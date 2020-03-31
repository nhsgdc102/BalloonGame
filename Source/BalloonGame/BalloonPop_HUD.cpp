// Copyright 2020 Northview High School Game Development Club

#include "BalloonPop_HUD.h"

ABalloonPop_HUD::ABalloonPop_HUD()
{
	fadeDuration = 4.f;
	fadeRate = 1.f / fadeDuration;
	fadeOpacity = 1.f;
	fadeDirection = -1.f;
	bIsFading = false;
	screenSize = FVector2D();
}

void ABalloonPop_HUD::DrawHUD()
{
	Super::DrawHUD();

	if (bIsFading)
	{
		fadeRate = 1.f / fadeDuration; //recalculates in case it was changed in blueprints
		fadeOpacity += fadeDirection * fadeRate * GetWorld()->GetDeltaSeconds(); //gets new opacity
		fadeOpacity = FMath::Clamp<float>(fadeOpacity, 0.f, 1.f); //from 0 to 1
		FLinearColor color = FLinearColor(0.f, 0.f, 0.f, fadeOpacity); //black color with fadeOpacity for alpha

		//stopping fade
		if (fadeDirection < 0)
			bIsFading = !(fadeOpacity == 0.f); //fade in stops when opacity reaches 0
		//fade out should not stop so that program continues to draw black rectangle

		GetWorld()->GetGameViewport()->GetViewportSize(screenSize); //puts screen size in screenSize
		DrawRect(color, 0.f, 0.f, screenSize.X, screenSize.Y);
	}
}

void ABalloonPop_HUD::fadeIn()
{
	bIsFading = true;
	fadeOpacity = 1.f; //initially black
	fadeDirection = -1.f; //decreases opacity of black screen; more transparent
}

void ABalloonPop_HUD::fadeOut()
{
	bIsFading = true;
	fadeOpacity = 0.f; //initially transparent
	fadeDirection = 1.f; //increases opacity of black screen; less transparent
}