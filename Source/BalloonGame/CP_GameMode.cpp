// Copyright 2020 Northview High School Game Development Club


#include "CP_GameMode.h"
#include "Engine/World.h"
#include "CP_GameInstance.h"

// Constructor
ACP_GameMode::ACP_GameMode()
{
	/*Round Transition*/
	TotalRounds = 25;
	AvgRoundTime = 20.f;
	InitSpawnFrequency = 0.2f;
	CoeffSpawnFrequency = 0.01875f;
	InitMaxSpawnRadius = 200.f;
	CoeffMaxSpawnRadius = 40.f;
	InitMaxEscapedBalloons = 10;
	CoeffMaxEscapedBalloons = 2;
	ProbabilityTransitionMat = 
		// Sets up probability matrix with a generation coefficient of 0.85 (transposed so that transform vector function works)
		FMatrix(
			FVector(0.85f, 0.1275f, 0.0225f), // Row 1
			FVector(0.f, 0.85f, 0.15f), // Row 2
			FVector(0.f, 0.f, 1.f), // Row 3
			FVector(0.f) // Row 4 (all zero entries)
		);
}

// Called after game mode is instantiated
void ACP_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Gets starting state information from game instance
	const UCP_GameInstance* const GameInstanceObj = Cast<UCP_GameInstance, UGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstanceObj)
	{
		RoundNum = GameInstanceObj->GetInitRoundNumber();
		BalloonProbabilities = GameInstanceObj->GetInitBalloonProbabilities();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ACP_GameMode->Cast to UCP_GameInstance failed!"));
}

/*Round Transition*/
float ACP_GameMode::CalcSpawnFrequency() const { return InitSpawnFrequency + CoeffSpawnFrequency * RoundNum; }
float ACP_GameMode::CalcMaxSpawnRadius() const { return InitMaxSpawnRadius + CoeffMaxSpawnRadius * RoundNum; }
int32 ACP_GameMode::CalcMaxEscapedBalloons() const { return InitMaxEscapedBalloons + CoeffMaxEscapedBalloons * RoundNum; }

// Call when going to the next round
void ACP_GameMode::CalcNextBalloonProbabilities() 
{	
	// Transforms vector and returns a 4-dimensional vector
	FVector4 Temporary = ProbabilityTransitionMat.TransformVector(BalloonProbabilities);
	// Extracts the 3-dimensional portion of the vector
	BalloonProbabilities = FVector(Temporary.X, Temporary.Y, Temporary.Z);
}

/*Spawning Balloons*/
// Uses balloon probabilities to randomly generate a balloon type
EBalloonType ACP_GameMode::GetRandomBalloonType() const
{
	const float RandFloat = FMath::FRand();
	if (RandFloat <= BalloonProbabilities.X)
		return EBalloonType::SLOW;
	else if (RandFloat <= BalloonProbabilities.X + BalloonProbabilities.Y)
		return EBalloonType::MEDIUM;
	else
		return EBalloonType::FAST;
}

// Gets random spawn transform between min and max spawn radius
FTransform ACP_GameMode::GetRandomSpawnTransform(const float MinSpawnRadius, const float MaxSpawnRadius) const
{
	const float Angle = FMath::RandRange(0.f, 2.f*PI); //PI is automatically defined
	const float Radius = FMath::RandRange(MinSpawnRadius, MaxSpawnRadius);
	const FVector SpawnLocation = FVector(Radius*cosf(Angle), Radius*sinf(Angle), 0.f); // Converts to Cartesian from polar

	return FTransform
		(
			FRotator(0.f, 0.f, 0.f).Quaternion(), // Quat with zero rotation
			SpawnLocation 
		);// Scale defaults to <1,1,1>
}

/*Debug*/
// Prints the transition matrix to log
void ACP_GameMode::PrintTransitionMatrix() const
{
	FString Output = "";

	for (int i = 0; i < 3; i++) // Hard coded for printing first three rows (up to third entry) of matrix
	{
		for (int j = 0; j < 3; j++)
		{
			Output += TEXT(" ") + FString::SanitizeFloat(ProbabilityTransitionMat.M[i][j]);
		}
		Output += TEXT(",");
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *Output); // %s tells the log to include Output in that location, *Output returns the TCHAR*
}

// Prints the balloon probability vector to log
void ACP_GameMode::PrintBalloonProbabilities() const
{
	const FString Output = FString::Printf(TEXT("%2.4f %2.4f %2.4f"),
		BalloonProbabilities.X, BalloonProbabilities.Y, BalloonProbabilities.Z);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Output); // %s tells the log to include Output in that location, *Output returns the TCHAR*
}