// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CP_GameMode.generated.h"

/**
 * Enum for balloon types
 */
UENUM(BlueprintType)
enum class EBalloonType : uint8
{
	SLOW = 0	UMETA(DisplayName = "SLOW"),
	MEDIUM = 1	UMETA(DisplayName = "MEDIUM"),
	FAST = 2	UMETA(DisplayName = "FAST")
};

/**
 * 
 */
UCLASS()
class BALLOONGAME_API ACP_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Constructor
	ACP_GameMode();

	// Called after game mode is instantiated
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/*Debug*/
	UFUNCTION(BlueprintCallable)
		// Prints the transition matrix to log
		void PrintTransitionMatrix() const;
	UFUNCTION(BlueprintCallable)
		// Prints the balloon probability vector to log
		void PrintBalloonProbabilities() const;

protected:
	/*Round Transition*/
	UFUNCTION(BlueprintPure)
		float CalcSpawnFrequency() const;
	UFUNCTION(BlueprintPure)
		float CalcMaxSpawnRadius() const;
	UFUNCTION(BlueprintPure)
		int32 CalcMaxEscapedBalloons() const;

	UFUNCTION(BlueprintCallable)
		// Call when going to the next round
		void CalcNextBalloonProbabilities();

	/*Spawning Balloons*/
	UFUNCTION(BlueprintPure)
		// Uses balloon probabilities to randomly generate a balloon type
		EBalloonType GetRandomBalloonType() const;
	UFUNCTION(BlueprintPure)
		// Gets random spawn transform between min and max spawn radius
		FTransform GetRandomSpawnTransform(const float MinSpawnRadius, const float MaxSpawnRadius) const;

protected:
	/*Round Characteristics*/
	UPROPERTY(BlueprintReadWrite)
		int32 RoundNum;

private:
	/*Round Characteristics*/
	FVector BalloonProbabilities;

	/*Round Transition*/
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Number of rounds in game
		int32 TotalRounds;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Adjusts time (in seconds) in each round--used to calculate number of balloons per round
		float AvgRoundTime;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Initial value of spawn frequency
		float InitSpawnFrequency;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Increase in spawn frequency per round
		float CoeffSpawnFrequency;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Initial value of max spawn radius
		float InitMaxSpawnRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Increase in max spawn radius per round
		float CoeffMaxSpawnRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Initial value of max escaped balloons
		int32 InitMaxEscapedBalloons;
	UPROPERTY(EditDefaultsOnly, Category = "Round Transition")
		// Increase in max escaped balloons per round
		int32 CoeffMaxEscapedBalloons;

	// Stochastic matrix for calculating the next state of balloon probabilities
	FMatrix ProbabilityTransitionMat;
};
