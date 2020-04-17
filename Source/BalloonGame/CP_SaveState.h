// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CP_SaveState.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONGAME_API UCP_SaveState : public USaveGame
{
	GENERATED_BODY()
	
public:
	// Constructor--sets default values
	//UCP_SaveState();

	/*Setter Functions*/
	void SetRoundNumber(const int32 Val);
	void SetPlayerScore(const int32 Val);
	void SetEscapedBalloons(const int32 Val);
	void SetBalloonProbabilities(const FVector InVector);

	/*Getter Functions*/
	int32 GetRoundNumber() const;
	int32 GetPlayerScore() const;
	int32 GetEscapedBalloons() const;
	FVector GetBalloonProbabilities() const;
	static FString GetSlotName();
	static int32 GetUserIndex();

private:
	/*Data*/
	int32 RoundNum;
	int32 PlayerScore;
	int32 NumEscapedBalloons;
	FVector BalloonProbabilities;

	/*Metadata*/
	const static FString SaveSlotName;
	const static int32 UserIndex;
};
