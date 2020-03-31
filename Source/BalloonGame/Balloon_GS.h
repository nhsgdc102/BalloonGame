// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "Balloon_GS.generated.h"

//To be binded to in InfoDisplay Blueprint (UI) -- Prevents having to check every frame
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEscPercentSignature, float, pPercentEscaped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoundNumSignature, int32, pRoundNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerScoreSignature, int32, pPlayerScore);

/**
 * 
 */
UCLASS()
class BALLOONGAME_API ABalloon_GS : public AGameStateBase
{
	GENERATED_BODY()

public:
	//Sets default values
	ABalloon_GS();

private:
	//UI Variables
	int32 roundNum;
	float percentEscaped;
	int32 playerScore;

public:
	UPROPERTY(BlueprintAssignable, Category = "UpdateUI")
		FEscPercentSignature escapePercentageDelegate;
	UPROPERTY(BlueprintAssignable, Category = "UpdateUI")
		FRoundNumSignature roundNumDelegate;
	UPROPERTY(BlueprintAssignable, Category = "UpdateUI")
		FPlayerScoreSignature playerScoreDelegate;

public:
	UFUNCTION(BlueprintCallable)
		void updateEscapePercent(float pPercentEscaped);
	UFUNCTION(BlueprintCallable)
		//Calculates and updates escape percentage
		void updateEscapePercentage(int32 escaped, int32 maxEscaped);
	UFUNCTION(BlueprintCallable)
		void updateRoundNum(int32 pRoundNum);
	UFUNCTION(BlueprintCallable)
		void updatePlayerScore(int32 pPlayerScore);
	UFUNCTION(BlueprintCallable)
		//Calls functions above to update UI -- does not change its own values
		void updateUI(); 
};
