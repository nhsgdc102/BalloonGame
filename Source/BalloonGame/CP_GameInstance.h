// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CP_GameInstance.generated.h"

/*Forward declarations*/
class USaveGame;

/*Delegate declarations*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateRoundNumber, int32, RoundNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateScore, int32, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateEscapedBalloons, int32, EscapedBalloons);

/**
 * Stores all of the information needing to persist through level changes. Saves information not included in variables below when needed.
 */
UCLASS()
class BALLOONGAME_API UCP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Constructor--sets default values
	UCP_GameInstance();

	/*Saving high score and highest round*/
	// Saves best record when player dies
	void SaveBestRecord();

	UFUNCTION()
		// Called when a preexisting UCP_BestRecord object loads
		void OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	UFUNCTION()
		// Called when a UCP_BestRecord object is saved
		void OnAsyncSaveRecord(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	/*Setter functions*/
	void AddRoundNumber(int32 Val);
	void AddPlayerScore(int32 Val);
	void AddEscapedBalloons(int32 Val);

	/*Getter functions*/
	UFUNCTION(BlueprintPure)
		int32 GetRoundNumber() const;
	UFUNCTION(BlueprintPure)
		int32 GetPlayerScore() const;
	UFUNCTION(BlueprintPure)
		int32 GetEscapedBalloons() const;

private:
	// Helper function for async saving best record
	void AsyncSaveRecord(int32 pHighScore, int32 pHighestRound);

private:
	int32 RoundNum;
	int32 PlayerScore;
	int32 NumEscapedBalloons;

	/*Delegates*/
public:
	FUpdateRoundNumber RoundNumber_Delegate;
	FUpdateScore Score_Delegate;
	FUpdateEscapedBalloons EscapedBalloons_Delegate;
};
