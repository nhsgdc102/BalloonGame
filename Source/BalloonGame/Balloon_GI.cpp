// Copyright 2020 Northview High School Game Development Club


#include "Balloon_GI.h"

UBalloon_GI::UBalloon_GI()
{
	roundNum = 1;
	playerScore = 0;
	escapedBalloons = 0;
	maxEscapedBalloons = 10; 
	lookSensitivity = 0.5f;
}

void UBalloon_GI::Init()
{
	Super::Init();

	//Loads up existing play info
	if (UGameplayStatics::DoesSaveGameExist("highScoreSlot", 0))
	{
		UscoreSaver* saveObj = Cast<UscoreSaver>(UGameplayStatics::LoadGameFromSlot("highScoreSlot", 0));
		if (IsValid(saveObj))
			//UI will access this value upon Construct() and use it to change lookSensitivity in the player
			lookSensitivity = saveObj->playerLookSensitivity;
	}
}

int32 UBalloon_GI::getRoundNum() const
{
	return roundNum;
}

int32 UBalloon_GI::getPlayerScore() const
{
	return playerScore;
}

int32 UBalloon_GI::getEscapedBalloons() const
{
	return escapedBalloons;
}

float UBalloon_GI::getLookSensitivity() const
{
	return lookSensitivity;
}

void UBalloon_GI::setRoundNum(int32 pRoundNum)
{
	roundNum = pRoundNum;

	//Updating game state
	ABalloon_GS* gameState = Cast<ABalloon_GS>(UGameplayStatics::GetGameState(this));
	if (IsValid(gameState))
		gameState->updateRoundNum(roundNum);
}

void UBalloon_GI::addToScore(int32 addition)
{
	setScore(playerScore + addition);
}

void UBalloon_GI::setScore(int32 pScore)
{
	playerScore = pScore;

	//Updating game state
	ABalloon_GS* gameState = Cast<ABalloon_GS>(UGameplayStatics::GetGameState(this));
	if (!(gameState == NULL))
		gameState->updatePlayerScore(playerScore);
}

void UBalloon_GI::incrementEscaped()
{
	setEscapedBalloons(escapedBalloons + 1);

	//if the number of escapedBalloons exceeds max -> ends game
	if (escapedBalloons >= maxEscapedBalloons)
		if (EndGameDelegate.IsBound())
			EndGameDelegate.Execute();
}

void UBalloon_GI::setEscapedBalloons(int32 pEscapedBalloons)
{
	escapedBalloons = pEscapedBalloons;

	//Updating game state
	ABalloon_GS* gameState = Cast<ABalloon_GS>(UGameplayStatics::GetGameState(this));
	if (IsValid(gameState))
		gameState->updateEscapePercentage(escapedBalloons, maxEscapedBalloons);
}

void UBalloon_GI::setMaxEscapedBalloons(int32 max)
{
	maxEscapedBalloons = max;

	//Updating game state
	ABalloon_GS* gameState = Cast<ABalloon_GS>(UGameplayStatics::GetGameState(this));
	if (!(gameState == NULL))
		gameState->updateEscapePercentage(escapedBalloons, maxEscapedBalloons);
}

void UBalloon_GI::setLookSensitivity(float pLookSensitivity)
{
	lookSensitivity = pLookSensitivity;
}