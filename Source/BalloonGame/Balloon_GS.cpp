// Copyright 2020 Northview High School Game Development Club


#include "Balloon_GS.h"

ABalloon_GS::ABalloon_GS()
{
	roundNum = 1;
	percentEscaped = 0.f;
	playerScore = 0;
}

void ABalloon_GS::updateEscapePercent(float pPercentEscaped)
{
	percentEscaped = pPercentEscaped;
	if (escapePercentageDelegate.IsBound())
		escapePercentageDelegate.Broadcast(percentEscaped);
}

void ABalloon_GS::updateEscapePercentage(int32 escaped, int32 maxEscaped)
{
	percentEscaped = ((float) escaped) / maxEscaped;
	if (escapePercentageDelegate.IsBound())
		escapePercentageDelegate.Broadcast(percentEscaped);
}

void ABalloon_GS::updateRoundNum(int32 pRoundNum)
{
	roundNum = pRoundNum;
	if (roundNumDelegate.IsBound())
		roundNumDelegate.Broadcast(roundNum);
}

void ABalloon_GS::updatePlayerScore(int32 pPlayerScore)
{
	playerScore = pPlayerScore;
	if (playerScoreDelegate.IsBound())
		playerScoreDelegate.Broadcast(playerScore);
}

void ABalloon_GS::updateUI()
{
	updateEscapePercent(percentEscaped);
	updateRoundNum(roundNum);
	updatePlayerScore(playerScore);
}