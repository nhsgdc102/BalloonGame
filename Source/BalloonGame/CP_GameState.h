// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CP_GameState.generated.h"

/**
 * 
 */
UCLASS()
class BALLOONGAME_API ACP_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void SetMaxEscapedBalloons(const int32 InMaxEscapedBalloons);

protected:
	// Called when game begins
	virtual void BeginPlay() override;

private:
	int32 PlayerScore;
	int32 NumEscapedBalloons;
	int32 MaxEscapedBalloons;
};
