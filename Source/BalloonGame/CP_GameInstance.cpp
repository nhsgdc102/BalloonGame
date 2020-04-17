// Copyright 2020 Northview High School Game Development Club


#include "CP_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "CP_SaveState.h"

// Constructor--sets default values
UCP_GameInstance::UCP_GameInstance()
{
	/*Default values -- used for new games*/
	InitRoundNum = 0;
	InitPlayerScore = 0;
	InitNumEscapedBalloons = 0;
	InitBalloonProbabilities = FVector(1.f, 0.f, 0.f);
}

// Loading data function--to be called from main menu widget
bool UCP_GameInstance::LoadSaveState()
{
	/*Synchronously loads CP_SaveState object*/
	UCP_SaveState* const LoadedObj = Cast<UCP_SaveState, USaveGame>(
		UGameplayStatics::LoadGameFromSlot(UCP_SaveState::GetSlotName(), UCP_SaveState::GetUserIndex()));
	if (LoadedObj)
	{
		/*Copies over data*/
		InitRoundNum = LoadedObj->GetRoundNumber();
		InitPlayerScore = LoadedObj->GetPlayerScore();
		InitNumEscapedBalloons = LoadedObj->GetEscapedBalloons();
		InitBalloonProbabilities = LoadedObj->GetBalloonProbabilities();

		/*Deletes loaded save object (This does not point to the object in the memory slot)*/
		LoadedObj->BeginDestroy();
		
		return true;
	}
	else
		return false;
}

/*Getter functions*/
int32 UCP_GameInstance::GetInitRoundNumber() const { return InitRoundNum; }
int32 UCP_GameInstance::GetInitPlayerScore() const { return InitPlayerScore; }
int32 UCP_GameInstance::GetInitEscapedBalloons() const { return InitNumEscapedBalloons; }
FVector UCP_GameInstance::GetInitBalloonProbabilities() const { return InitBalloonProbabilities; }

/*
/*Saving high score and highest round
// Saves best record when player dies
void UCP_GameInstance::SaveBestRecord()
{
	/**
	* https://docs.unrealengine.com/en-US/Gameplay/SaveGame/Code/index.html
	
	FString SlotName = UCP_BestRecord::GetSlotName();
	int32 UserIndex = UCP_BestRecord::GetUserIndex();

	// Checking if an another UCP_BestRecord object already exists
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		// Async loading
		FAsyncLoadGameFromSlotDelegate LoadedDelegate;
		LoadedDelegate.BindUObject(this, &UCP_GameInstance::OnAsyncLoadRecord);
		UGameplayStatics::AsyncLoadGameFromSlot(SlotName, UserIndex, LoadedDelegate);
	}
	else
		// Sets current attributes as the best
		AsyncSaveRecord(PlayerScore, RoundNum);
}

// Called when a preexisting UCP_BestRecord object loads
void UCP_GameInstance::OnAsyncLoadRecord(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UCP_BestRecord* ExistingSaveGameObj = Cast<UCP_BestRecord, USaveGame>(LoadedGameData);
	if (IsValid(ExistingSaveGameObj))
	{
		int32 ExistingHighScore = ExistingSaveGameObj->GetHighScore();
		int32 ExistingHighestRound = ExistingSaveGameObj->GetHighestRound();

		int32 HighScore = FMath::Max<int32>(ExistingHighScore, PlayerScore);
		int32 HighestRound = FMath::Max<int32>(ExistingHighestRound, RoundNum);

		// Checking if anything needs to be saved (if any change happened)
		if (HighScore != ExistingHighScore || HighestRound != ExistingHighestRound)
			AsyncSaveRecord(HighScore, HighestRound);
	}
}

// Helper function for async saving best record
void UCP_GameInstance::AsyncSaveRecord(int32 HighScore, int32 HighestRound)
{
	// Creating new UCP_BestRecord object
	UCP_BestRecord* SaveObject = Cast<UCP_BestRecord, USaveGame>
		(UGameplayStatics::CreateSaveGameObject(UCP_BestRecord::StaticClass()));

	if (IsValid(SaveObject))
	{
		// Storing informating inside UCP_BestRecord object
		SaveObject->SetHighScore(HighScore);
		SaveObject->SetHighestRound(HighestRound);

		FAsyncSaveGameToSlotDelegate SavedDelegate;
		SavedDelegate.BindUObject(this, &UCP_GameInstance::OnAsyncSaveRecord);

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveObject, UCP_BestRecord::GetSlotName(),
			UCP_BestRecord::GetUserIndex(), SavedDelegate);
	}
}

// Called when a UCP_BestRecord object is saved
void UCP_GameInstance::OnAsyncSaveRecord(const FString& SlotName, const int32 UserIndex, bool bSuccess) {}

/*Mutator functions
void UCP_GameInstance::AddRoundNumber(int32 Val)
{
	RoundNum += Val;
	RoundNumber_Delegate.Broadcast(RoundNum);
}
void UCP_GameInstance::AddPlayerScore(int32 Val)
{
	PlayerScore += Val;
	Score_Delegate.Broadcast(PlayerScore);
}
void UCP_GameInstance::AddEscapedBalloons(int32 Val)
{
	NumEscapedBalloons += Val;
	EscapedBalloons_Delegate.Broadcast(NumEscapedBalloons);
}

/*Getter functions
int32 UCP_GameInstance::GetRoundNumber() const { return RoundNum; }
int32 UCP_GameInstance::GetPlayerScore() const { return PlayerScore; }
int32 UCP_GameInstance::GetEscapedBalloons() const { return NumEscapedBalloons; }
*/
