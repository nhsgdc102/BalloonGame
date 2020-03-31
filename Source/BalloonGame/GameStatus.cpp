// Copyright 2020 Northview High School Game Development Club

#include "GameStatus.h"
#include "BalloonGame.h"

UGameStatus::UGameStatus()
{
	roundNum = 1;
	playerScore = 0;
	balProb = FMatrix();
	spawnFrequency = 0.f;
	escapedBalloons = 0;
	currentLevel = FName(TEXT("Jungle_L"));
	saveSlotName = FString(TEXT("gameStatusSlot"));
	userIndex = 0;
}

FName UGameStatus::getSavedLevelName() const
{
	return this->currentLevel;
}