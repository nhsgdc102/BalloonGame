// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h" //For getting game state
#include "Balloon_GS.h" //For accessing game state
#include "scoreSaver.h" //For accessing scoreSaver obj

#include "Balloon_GI.generated.h"

/**
 * 
 */

//To be binded to endGame in game mode
DECLARE_DYNAMIC_DELEGATE(FEndGameSignature);

UCLASS()
class BALLOONGAME_API UBalloon_GI : public UGameInstance
{
	GENERATED_BODY()

public:
	//Constructor--sets default values
	UBalloon_GI(); 

	//Init is equivalent to begin play except it happens before
	//Sets up initial lookSensitivity
	virtual void Init() override;

public:
	FEndGameSignature EndGameDelegate;

private:
	int32 roundNum;
	int32 playerScore;
	int32 escapedBalloons;
	int32 maxEscapedBalloons; //For calculating escape percentage
	float lookSensitivity;

public:
	//Getter functions
	UFUNCTION(BlueprintPure)
		int32 getRoundNum() const;
	UFUNCTION(BlueprintPure)
		int32 getPlayerScore() const;
	int32 getEscapedBalloons() const;
	UFUNCTION(BlueprintPure)
		float getLookSensitivity() const;

	//Setter functions
	void setRoundNum(int32 pRoundNum);

	UFUNCTION(BlueprintCallable)
		//Adds to player score and updates game state
		void addToScore(int32 addition);

	void setScore(int32 pScore);

	UFUNCTION(BlueprintCallable)
		//Adds 1 to escaped balloons and updates game state
		void incrementEscaped();

	//For use by game mode upon loading up game status
	void setEscapedBalloons(int32 pEscapedBalloons);

	//Sets maxEscapedBalloons and updates game state
	void setMaxEscapedBalloons(int32 max);

	UFUNCTION(BlueprintCallable)
		//Sets lookSensitivity
		void setLookSensitivity(float pLookSensitivity);
};
