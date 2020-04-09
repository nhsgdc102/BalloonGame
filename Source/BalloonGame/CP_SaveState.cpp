// Copyright 2020 Northview High School Game Development Club


#include "CP_SaveState.h"

/*Static variable initialization*/
const FString UCP_SaveState::SaveSlotName(TEXT("SaveStateSlot"));
const int32 UCP_SaveState::UserIndex = 0;

// Constructor--sets default values
UCP_SaveState::UCP_SaveState()
{
	RoundNum = 1;
	PlayerScore = 0;
	SpawnProbabilityMat = FMatrix();
	NumEscapedBalloons = 0;
	LevelOfPlay = 0;
}

/*Setter Functions*/
void UCP_SaveState::SetRoundNumber(int32 Val) { RoundNum = Val; }
void UCP_SaveState::SetPlayerScore(int32 Val) { PlayerScore = Val; }
void UCP_SaveState::SetEscapedBalloons(int32 Val) { NumEscapedBalloons = Val; }
void UCP_SaveState::SetSpawnProbabilityMatrix(FMatrix Matrix) { SpawnProbabilityMat = Matrix; }
void UCP_SaveState::SetLevelOfPlay(int32 LevelIndex) { LevelOfPlay = LevelIndex; }

/*Getter Functions*/
int32 UCP_SaveState::GetRoundNumber() const { return RoundNum; }
int32 UCP_SaveState::GetPlayerScore() const { return PlayerScore; }
int32 UCP_SaveState::GetEscapedBalloons() const { return NumEscapedBalloons; }
FMatrix UCP_SaveState::GetSpawnProbabilityMatrix() const { return SpawnProbabilityMat; }
int32 UCP_SaveState::GetLevelOfPlay() const { return LevelOfPlay; }
FString UCP_SaveState::GetSlotName() { return SaveSlotName; }
int32 UCP_SaveState::GetUserIndex() { return UserIndex; }

