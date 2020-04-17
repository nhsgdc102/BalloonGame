// Copyright 2020 Northview High School Game Development Club


#include "CP_GameState.h"
#include "Engine/World.h"
#include "CP_GameInstance.h"

// Called when game begins
void ACP_GameState::BeginPlay()
{
	Super::BeginPlay();

	// Gets starting state information from game instance
	const UCP_GameInstance* const GameInstanceObj = Cast<UCP_GameInstance, UGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstanceObj)
	{
		PlayerScore = GameInstanceObj->GetInitPlayerScore();
		NumEscapedBalloons = GameInstanceObj->GetInitEscapedBalloons();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ACP_GameState->Cast to UCP_GameInstance failed!"));
}

void ACP_GameState::SetMaxEscapedBalloons(const int32 InMaxEscapedBalloons) { MaxEscapedBalloons = InMaxEscapedBalloons; }