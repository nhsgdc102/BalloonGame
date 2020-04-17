// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CP_GameInstance.generated.h"

/**
 * Stores information needed by game mode and game state to initialize the game
 */
UCLASS()
class BALLOONGAME_API UCP_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Constructor--sets default values
	UCP_GameInstance();

	// Loading data function--to be called from main menu widget
	UFUNCTION(BlueprintCallable)
		bool LoadSaveState();

	/*Getter functions*/
	int32 GetInitRoundNumber() const;
	int32 GetInitPlayerScore() const;
	int32 GetInitEscapedBalloons() const;
	FVector GetInitBalloonProbabilities() const;
	
private:
	int32 InitRoundNum;
	int32 InitPlayerScore;
	int32 InitNumEscapedBalloons;
	FVector InitBalloonProbabilities;

/*
	/*Saving high score and highest round
	// Saves best record when player dies
	void SaveBestRecord();

	UFUNCTION()
		// Called when a preexisting UCP_BestRecord object loads
		void OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	UFUNCTION()
		// Called when a UCP_BestRecord object is saved
		void OnAsyncSaveRecord(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	/*Setter functions
	void AddRoundNumber(int32 Val);
	void AddPlayerScore(int32 Val);
	void AddEscapedBalloons(int32 Val);

	/*Getter functions
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

	/*Delegates
public:
	FUpdateRoundNumber RoundNumber_Delegate;
	FUpdateScore Score_Delegate;
	FUpdateEscapedBalloons EscapedBalloons_Delegate;
*/
};
