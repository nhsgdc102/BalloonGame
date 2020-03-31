// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameStatus.generated.h"

/**
 * 
 */

UCLASS()
class BALLOONGAME_API UGameStatus : public USaveGame
{
	GENERATED_BODY()

public:
	//Sets default values
	UGameStatus();

	UFUNCTION(BlueprintPure)
		FName getSavedLevelName() const;

public:
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		int32 roundNum;
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		int32 playerScore;
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		FMatrix balProb;
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		float spawnFrequency;
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		int32 escapedBalloons;
	UPROPERTY(BlueprintReadWrite, Category = "StatusData")
		FName currentLevel;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game ID Vars")
		FString saveSlotName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Game ID Vars")
		int32 userIndex;
};
