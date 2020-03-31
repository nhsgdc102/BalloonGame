// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "scoreSaver.generated.h"

/**
 * 
 */

UCLASS()
class BALLOONGAME_API UscoreSaver : public USaveGame
{
	GENERATED_BODY()
	
public:
	//sets default values
	UscoreSaver();

public:
	UPROPERTY(BlueprintReadWrite, Category = "SavedData")
		int32 highScore;
	UPROPERTY(BlueprintReadWrite, Category = "SavedData")
		int32 highestRound;
	UPROPERTY(BlueprintReadWrite, Category = "SavedData")
		float playerLookSensitivity;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game ID Vars")
		FString saveSlotName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Game ID Vars")
		int32 userIndex;
};
