// Copyright 2020 Northview High School Game Development Club


#include "CP_BestRecord.h"

/*Static variable initialization*/
const FString UCP_BestRecord::SaveSlotName(TEXT("BestRecordSlot"));
const int32 UCP_BestRecord::UserIndex = 0;

// Constructor--sets default values
UCP_BestRecord::UCP_BestRecord()
{
	HighScore = 0;
	HighestRound = 0;
}

/*Setter Functions*/
void UCP_BestRecord::SetHighScore(int32 Val) { HighScore = Val; }
void UCP_BestRecord::SetHighestRound(int32 Val) { HighestRound = Val; }

/*Getter Functions*/
int32 UCP_BestRecord::GetHighScore() const { return HighScore; }
int32 UCP_BestRecord::GetHighestRound() const { return HighestRound; }
FString UCP_BestRecord::GetSlotName() { return SaveSlotName; }
int32 UCP_BestRecord::GetUserIndex() { return UserIndex; }

