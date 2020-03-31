// Copyright 2020 Northview High School Game Development Club

#include "scoreSaver.h"
#include "BalloonGame.h"

UscoreSaver::UscoreSaver()
{
	highScore = 0;
	highestRound = 0;
	playerLookSensitivity = 0.5f;
	saveSlotName = FString(TEXT("highScoreSlot"));
	userIndex = 0;
}
