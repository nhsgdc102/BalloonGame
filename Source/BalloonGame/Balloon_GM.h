// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/Engine.h"
//Balloon class
#include "Balloon.h"
//World timer manager
#include "TimerManager.h"
//Slingshot
#include "Slingshot.h"
//Saving playthrough data
#include "scoreSaver.h"
//Saving game status between rounds
#include "GameStatus.h"
#include "Kismet/GameplayStatics.h"
#include "Balloon_GI.h" //Game instance class

#include "Balloon_GM.generated.h"

/**
 *
 init prefix is for const variable

 */
UENUM(BlueprintType)
enum class EBalloonType : uint8
{
	BT_Slow		UMETA(DisplayName = "Slow"),
	BT_Medium	UMETA(DisplayName = "Medium"),
	BT_Fast		UMETA(DisplayName = "Fast")
};
//Source - https://wiki.unrealengine.com/Enums_For_Both_C%2B%2B_and_BP

UCLASS()
class BALLOONGAME_API ABalloon_GM : public AGameModeBase
{
	GENERATED_BODY()

public:
	//Sets default values
	ABalloon_GM();

protected:
	void BeginPlay();

//General
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//Initial value of maxEscapedBalloons--used for calculating maxEscapedBalloons
		int initMaxEscapedBalloons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//Maximum balloons that can escape before game over
		int maxEscapedBalloons;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//Increase in maximum escaped balloons per round--to make game easier
		int maxEscapedIncrease;

//Balloon Type Probabilities
private:
	//For transforming balloon probability vector; treated like a 3x3 matrix
	FMatrix transMat;
	//Stores the spawn probability values for each type; values in first column form a vector
	FMatrix balProb; 

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//Coefficient for generating transMat values
		float genCoeff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//Must be less than or equal to 4 in length; stores initial spawn probabilities of balloon types
		TArray<float> probArray;

private:
	//Number of types of balloons, set equal to length of probArray
	int numOfTypes; 

private:
	//Generates a transformation matrix for calculating spawn probabilities (stochastic matrix)
	void genTransMat(float coeff);
	//Sets probability values into first column of balProb (stochastic vector)
	void setBalProb(TArray<float> prob);

//Spawning Balloons
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//Initial value of spawnRadius--used in linear calculation for spawnRadius
		float initSpawnRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//Distance from center beyond which balloons cannot spawn
		float spawnRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//Distance from center inside which balloons cannot spawn 
		float minRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//Z-value of balloon spawn location--relative to player pawn
		float defaultZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//slowest class for balloon
		TSubclassOf<ABalloon> slowBalloonType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//medium fastest class for balloon
		TSubclassOf<ABalloon> mediumBalloonType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//fastest class for balloon
		TSubclassOf<ABalloon> fastBalloonType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//number of balloons left to spawn in the round
		int numBalloonsLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//The range above and below to randomize the period
		float periodPlusOrMinus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//Decides whether or not to start spawning cycle
		bool bSpawnBalloons;

public:
	//Binded to spawnBalloon()--used to have a delay between spawns
	FTimerHandle spawnTimerHandle;

public:
	UFUNCTION(BlueprintCallable)
		//Returns randomized transform
		FTransform getRandomSpawn() const;
	UFUNCTION(BlueprintCallable)
		//Returns randomly one of the balloon types()
		EBalloonType getRandBalloonType() const;
	UFUNCTION(BlueprintCallable)
		//spawns balloon of randomized type and transform
		void spawnBalloon();
	UFUNCTION(BlueprintCallable)
		//Activates spawnTimerHandle
		void startSpawnTimer();
	UFUNCTION()
		//Sees if last balloon of round is dead
		void onBalloonDeath(AActor* DestroyedActor);

//Round Characteristics
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Current round number
		int roundNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Adjusts time in each round--used to calculate balLimit (in seconds)
		float avgRoundTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//maximum number of rounds in game
		int maxRounds;
	UPROPERTY(BlueprintReadWrite, Category = "RoundChars")
		//max number of balloons in round; calculating using frequency
		int balLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Maximum value of balLimit
		int maxBalloonsPerRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Initial value of spawnFrequency--used in linear function for spawnFrequency
		float initSpawnFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Number of balloons per second
		float spawnFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Increase in frequency per round
		float freqIncrease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Maximum value of frequency
		float maxFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Increase in spawnRadius per round
		float radiusIncrease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoundChars")
		//Maximum value for spawnRadius
		float maxSpawnRadius;
public:
	UFUNCTION(BlueprintCallable)
		//Sets values for next round
		void setupNextRound();

private:
	int getNextMax(float frequency) const; //Gets value for maxBalloons for next round
	float getNextFrequency() const; //Gets value for spawnFrequency for next round
	void setNextProbVector(); //Uses matrix multiplication to set next prob array
	float getNextSpawnRadius() const;
	int getNextMaxEscapedBalloons() const;

//Level Transitions
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelTransitions")
		//number of sublevels
		int numLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelTransitions")
		//total time of transition
		float timeBetweenLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelTransitions")
		//delay before starting sublevel transition
		float transitionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelTransitions")
		//Stores names of levels; set number of elements equal to numLevels
		TArray<FName> levelArray;
private:
	//Binded to startLevelTransition; will perform the transition after a delay
	FTimerHandle startTransitionHandle;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Miscellaneous")
		//Moves game to next level (no C++ definition)
		void startLevelTransition();
	UFUNCTION(BlueprintPure)
		//Gets correct index in level array
		int32 getLevelIndex() const;

//Miscellaneous
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		//True if the player completed all possible rounds
		bool bWinner;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Miscellaneous")
		//Saves game and creates EndGameWidget (no C++ definition)
		void endGame(); 
	UFUNCTION(BlueprintCallable, Category = "Miscellaneous")
		//Saves play information and quits
		void quitToMenu();
	UFUNCTION(BlueprintCallable, Category = "Miscellaneous")
		//Saves play information and restarts
		void restartGame();

//Saving playthrough info
private:
	//Stores correct values for high score, highest round, and lookSensitivity
	UscoreSaver* saveObj;

public:
	UFUNCTION(BlueprintCallable)
		//playerScore should contain score of current playthrough
		void savePlayInformation();
	UFUNCTION()
		//Calculates high score and highest round
		void onAsyncLoadPlayInfo(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	UFUNCTION()
		//Reports success of save
		void onAsyncSavePlayInfo(const FString& SlotName, const int32 UserIndex, bool bSuccess);
private:
	//Handles details of saving play info
	void doAsyncSavePlay();

public:
	//Just to store in game mode for easy access from widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		//For easy access from widgets
		int32 gmHighestRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		//For easy access from widgets
		int32 gmHighScore;

//Saving game status info -- occurs between rounds
public:
	UFUNCTION(BlueprintCallable)
		//Called after setting up next level
		void saveGameStatus();
	UFUNCTION()
		//Reports success of save
		void onAsyncSaveGameStatus(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	UFUNCTION()
		//Adjusts current round characteristics to match those in existing gameStatus
		void onAsyncLoadGameStatus(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

private:
	//Handles details of saving gameStatus
	void doAsyncSaveGameStatus();
	//sets up all round chars according to new values from existing game status
	void calculateRoundChars(); 

private:
	//Stores current status details
	UGameStatus* statusObj;

//Debugging
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintTransMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bDebugProbVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		//Chars means characteristicss
		bool bPrintRoundChars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bDebugSpawnTransform;

private:
	void printTransMat();

};


