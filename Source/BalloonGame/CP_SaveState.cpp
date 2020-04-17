// Copyright 2020 Northview High School Game Development Club


#include "CP_SaveState.h"

/*Static variable initialization*/
const FString UCP_SaveState::SaveSlotName(TEXT("SaveStateSlot"));
const int32 UCP_SaveState::UserIndex = 0;
/*
// Constructor--sets default values
UCP_SaveState::UCP_SaveState()
{
	RoundNum = 1;
	PlayerScore = 0;
	NumEscapedBalloons = 0;
	BalloonProbabilities = FVector();
}
*/

/*Setter Functions*/
void UCP_SaveState::SetRoundNumber(const int32 Val) { RoundNum = Val; }
void UCP_SaveState::SetPlayerScore(const int32 Val) { PlayerScore = Val; }
void UCP_SaveState::SetEscapedBalloons(const int32 Val) { NumEscapedBalloons = Val; }
void UCP_SaveState::SetBalloonProbabilities(const FVector InVector) { BalloonProbabilities = InVector; }

/*Getter Functions*/
int32 UCP_SaveState::GetRoundNumber() const { return RoundNum; }
int32 UCP_SaveState::GetPlayerScore() const { return PlayerScore; }
int32 UCP_SaveState::GetEscapedBalloons() const { return NumEscapedBalloons; }
FVector UCP_SaveState::GetBalloonProbabilities() const { return BalloonProbabilities; }
FString UCP_SaveState::GetSlotName() { return SaveSlotName; }
int32 UCP_SaveState::GetUserIndex() { return UserIndex; }

