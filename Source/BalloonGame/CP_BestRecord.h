// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CP_BestRecord.generated.h"

/**
 *
 */
UCLASS()
class BALLOONGAME_API UCP_BestRecord : public USaveGame
{
	GENERATED_BODY()

public:
	// Constructor--sets default values
	UCP_BestRecord();

	/*Setter Functions*/
	void SetHighScore(int32 Val);
	void SetHighestRound(int32 Val);

	/*Getter Functions*/
	int32 GetHighScore() const;
	int32 GetHighestRound() const;
	static FString GetSlotName();
	static int32 GetUserIndex();

private:
	/*Data*/
	int32 HighScore;
	int32 HighestRound;

	/*Metadata*/
	const static FString SaveSlotName;
	const static int32 UserIndex;
};
