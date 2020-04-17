// Fill out your copyright notice in the Description page of Project Settings.

#include "Balloon_GM.h"

ABalloon_GM::ABalloon_GM()
{
	initMaxEscapedBalloons = 10;
	maxEscapedBalloons = 10;
	maxEscapedIncrease = 2;

	genCoeff = 0.85f;

	initSpawnRadius = 200.f;
	spawnRadius = 200.f;
	minRadius = 150.f;
	defaultZ = -88.f;
	numBalloonsLeft = 1;
	periodPlusOrMinus = 0.1f;
	bSpawnBalloons = true;

	roundNum = 1;
	avgRoundTime = 20.f;
	maxRounds = 25;
	balLimit = 0;
	initSpawnFrequency = .2f;
	spawnFrequency = .2f; //1 balloon for every 5 seconds
	freqIncrease = 0.01875f;
	maxBalloonsPerRound = 100;
	maxFrequency = 1.4f;
	radiusIncrease = 40.f;
	maxSpawnRadius = 1000.f;

	numLevels = 5;
	timeBetweenLevels = 10.f;
	transitionDelay = 0.2f;

	bWinner = false;
	
	gmHighestRound = 0;
	gmHighScore = 0;

	bPrintTransMat = true;
	bDebugProbVector = true;
	bPrintRoundChars = true;
	bDebugSpawnTransform = true;
}

void ABalloon_GM::BeginPlay()
{
	//Binding endGame to game instance's endGame
	UBalloon_GI* gameInstance = Cast<UBalloon_GI>(UGameplayStatics::GetGameInstance(this));
	if (IsValid(gameInstance))
	{
		gameInstance->EndGameDelegate.BindDynamic(this, &ABalloon_GM::endGame);
	}

	numOfTypes = probArray.Num();
	genTransMat(genCoeff);
	setBalProb(probArray);

	balLimit = getNextMax(spawnFrequency);
	numBalloonsLeft = balLimit; //Sets numBalloonsLeft to the total number of balloons to be spawned in the round

	//Checking to see if game status exists (meaning there is already a saved game) -- also used after level transitions to restore info
	FString slotName = TEXT("gameStatusSlot");
	int32 userIndex = 0;
	if (UGameplayStatics::DoesSaveGameExist(slotName, userIndex))
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, TEXT("SaveGameExists"));

		//Async loading
		FAsyncLoadGameFromSlotDelegate LoadedDelegate;
		LoadedDelegate.BindUObject(this, &ABalloon_GM::onAsyncLoadGameStatus);
		UGameplayStatics::AsyncLoadGameFromSlot(slotName, userIndex, LoadedDelegate);
	}
	else {
		//if gameStatus exists, spawn timer will be called in onAsyncLoad

		//Deciding whether or not to start spawning cycle
		if (bSpawnBalloons)
			startSpawnTimer();
	}

	//Debugging
	if (bPrintTransMat)
		printTransMat();
}

void ABalloon_GM::genTransMat(float coeff)
{
	float runSum = 0.f;

	for (int j = 0; j < numOfTypes; j++)
	{
		runSum = 0.f; //resets runSum at beginning of each column

		for (int i = j; i < (numOfTypes - 1); i++)
		{
			transMat.M[i][j] = coeff * (1 - runSum);
			runSum += transMat.M[i][j];
		}
		//For last element in each column //To fill up space needed to reach 1
		transMat.M[numOfTypes - 1][j] = 1 - runSum;
	}
}

void ABalloon_GM::setBalProb(TArray<float> prob)
{
	for (int i = 0; i < numOfTypes; i++)
		balProb.M[i][0] = prob[i];
}

void ABalloon_GM::printTransMat()
{
	FString message;

	for (int i = (numOfTypes - 1); i >= 0; i--)
	{
		message = TEXT("");
		for (int j = 0; j < numOfTypes; j++)
		{
			message += (TEXT(" ") + FString::SanitizeFloat(transMat.M[i][j]));
		}
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, message);
	}
}

void ABalloon_GM::startSpawnTimer()
{
	//Gets time interval/period between balloon spawns
	float period = 1.f / spawnFrequency; //standard formula for period
	period = FMath::RandRange(period - periodPlusOrMinus, period + periodPlusOrMinus); //randomizes period in range

	GetWorldTimerManager().SetTimer(spawnTimerHandle, this, &ABalloon_GM::spawnBalloon, 1.f, false, period);
}

void ABalloon_GM::spawnBalloon()
{
	TSubclassOf<ABalloon> randBalloonType; //type to spawn balloon from; chosen from the available classes of balloons

	switch (getRandBalloonType())
	{
	case EBalloonType2::BT_Slow:
		randBalloonType = slowBalloonType;
		break;
	case EBalloonType2::BT_Medium:
		randBalloonType = mediumBalloonType;
		break;
	case EBalloonType2::BT_Fast:
		randBalloonType = fastBalloonType;
		break;
	}

	FTransform spawnTransform = getRandomSpawn();
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding; //Prevents balloons from spawning on top of each other

	AActor* spawnedBalloon = GetWorld()->SpawnActor(randBalloonType, &spawnTransform, spawnParams);
	if (IsValid(spawnedBalloon)) //is true if balloon did spawn
	{
		numBalloonsLeft--;
		if (numBalloonsLeft == 0) //Means that the last balloon of the round was just spawned
		{
			//lastBalloon = spawnedBalloon;
			//Implementing binding
			spawnedBalloon->OnDestroyed.AddDynamic(this, &ABalloon_GM::onBalloonDeath);
		}
		else //starts timer to spawn next balloon in the same round
			startSpawnTimer();
	}
}

FTransform ABalloon_GM::getRandomSpawn() const
{
	FVector loc = FVector();

	float theta = FMath::RandRange(0.f, 2.f*PI); //PI is automatically defined
	float r = FMath::RandRange(minRadius, spawnRadius);
	loc.X = r * cosf(theta);
	loc.Y = r * sinf(theta);
	loc.Z = defaultZ;

	FRotator zeroRotation = FRotator(0.f, 0.f, 0.f); //Default quaternion has 90 pitch, 180 roll
	FTransform trans = FTransform(zeroRotation.Quaternion(), loc); //Scale is default to <1,1,1>

	//totalBalloonCount++;
	if (bDebugSpawnTransform)
	{
		//FString message = "Balloon " + FString::FromInt(totalBalloonCount);
		FString message = "\nRadius: " + FString::SanitizeFloat(r);
		theta *= 360.f / (2.f*PI); //Radians to degrees
		message += "\nAngle: " + FString::SanitizeFloat(theta);
		message += "\nRotation: " + trans.Rotator().ToString();
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, message);
	}

	return trans;

	/*
		int radius;
		std::cout << "Value For Radius: ";
		std::cin >> radius;
		int theta = rand() % 360;
		int r = rand() % radius;
		int x = rand() % radius;
		int rr = r ^ 2;
		int xx = x ^ 2;
		int yy = sqrt(rr - xx);
		int y = rand() % yy;
		std::cout << y;
	*/
}

EBalloonType2 ABalloon_GM::getRandBalloonType() const
{
	int index; //The index of the array that is randomly chosen
	EBalloonType2 BT_Type;

	//Randomizes value between 0 and 1;
	float randFloat = FMath::RandRange(0.f, 1.f);

	float modValue = 0.f; //Contains values from balProb; goes from 0 to 1 with intervals matching balProb values

	//converting probability vector was unnecessary; modValue tracks each value in the balProb and increments from 0
	//modValue replaces the implementation form for the probability vector
	for (int i = 0; i < numOfTypes; i++)
	{
		modValue += balProb.M[i][0];
		if (randFloat <= modValue)
		{
			index = i;
			i = numOfTypes; //Prevents index from being set again
		}
	}
	//Include text below in the function to generate the prob vector every round
	/*
	//After running loop, lastValue should be equal to sum, which should be equal to 1
	if(bDebugProbArray)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black,
			TEXT("ProbArray:: Sum Equal to ") + FString::SanitizeFloat(lastValue));
	*/
	//Chooses balloon type based on index
	switch (index) {
	case 0:
		BT_Type = EBalloonType2::BT_Slow;
		break;
	case 1:
		BT_Type = EBalloonType2::BT_Medium;
		break;
	case 2:
		BT_Type = EBalloonType2::BT_Fast;
		break;
	default:
		BT_Type = EBalloonType2::BT_Slow;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, TEXT("Not enough balloon types"));
		break;
	}
	return BT_Type;
}

void ABalloon_GM::onBalloonDeath(AActor* DestroyedActor)
{
	if (roundNum >= maxRounds) //true if game has been won
	{
		bWinner = true;
		//bEndTheGame = true;
		endGame();
	}
	else { //Game is still ongoing
		setupNextRound();
	}
}

void ABalloon_GM::setupNextRound()
{
	//Basic setup
	roundNum++;
	setNextProbVector();

	//Implements calculations
	calculateRoundChars();

	//Saves info of new round -- allows user to come back to this round when reopening game -- also allows game mode to find info after opening new map
	saveGameStatus();
}

float ABalloon_GM::getNextFrequency() const
{
	//Can make more complex if this is not sufficient
	//This is the linear recursive function
	//float freq = currValue + freqIncrease;

	//Linear explicit function
	float freq = initSpawnFrequency + freqIncrease*(roundNum-1); //initially (round = 1) equal to initSpawnFrequency
	return FMath::Clamp<float>(freq, 0.f, maxFrequency);
}

void ABalloon_GM::setNextProbVector()
{
	//recursive function
	balProb = transMat * balProb;
	
	if (bDebugProbVector)
	{
		FString message = "Balloon Probability Vector: <";
		float sum = 0.f; //Used to see if sum is or is not equal to 1

		for (int i = 0; i < numOfTypes; i++)
		{
			message += (FString::SanitizeFloat(balProb.M[i][0]) + TEXT(" "));
			sum += balProb.M[i][0];
		}
		message += TEXT(">");
		message += (TEXT("\nSum :: ") + FString::SanitizeFloat(sum));
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, message);
	}
}

float ABalloon_GM::getNextSpawnRadius() const
{
	//Can make more complex if this is not sufficient
	//This is the linear recursive function
	//float radius = currValue + radiusIncrease;

	//Linear explicit function
	float radius = initSpawnRadius + radiusIncrease * (roundNum - 1);
	return FMath::Clamp<float>(radius, 0.f, maxSpawnRadius);
}

int ABalloon_GM::getNextMax(float frequency) const
{
	float totalBalloons = frequency * avgRoundTime;
	int roundedTotal = FMath::CeilToInt(totalBalloons);
	return FMath::Clamp<int>(roundedTotal, 0, maxBalloonsPerRound);
}

int ABalloon_GM::getNextMaxEscapedBalloons() const
{
	int maxEscaped = initMaxEscapedBalloons + maxEscapedIncrease * (roundNum - 1);
	return maxEscaped;
}

void ABalloon_GM::quitToMenu()
{
	//Saving play information so that lookSensitivity will be stored permanently
	savePlayInformation();

	//No need to save play status because that is saved before every round

	//Opening MainMenu level will automatically quit from current game
	UGameplayStatics::OpenLevel(this, "MainMenu_L");
}

void ABalloon_GM::restartGame()
{
	//Saving play information so that lookSensitivity will be stored permanently
	savePlayInformation();

	//Destroying any existing playStatus info because that will interrupt restart
	if (UGameplayStatics::DoesSaveGameExist("gameStatusSlot", 0))
		UGameplayStatics::DeleteGameInSlot("gameStatusSlot", 0);

	//Opening first level (Jungle_L) will automatically reset interactions
	UGameplayStatics::OpenLevel(this, "Jungle_L");
}

void ABalloon_GM::savePlayInformation()
{
	/*https://docs.unrealengine.com/en-US/Gameplay/SaveGame/Code/index.html*/
	//saveObj already declared in header file! -> to allow access from other methods
	
	//creating saveGame object
	saveObj = Cast<UscoreSaver>(UGameplayStatics::CreateSaveGameObject(UscoreSaver::StaticClass()));
	if (IsValid(saveObj)) //makes sure reference is not null
	{
		FString slotName = saveObj->saveSlotName;
		int32 userIndex = saveObj->userIndex;

		//Storing information for later use (from other methods or this method)
		saveObj->highestRound = roundNum;
		UBalloon_GI* gameInstance = Cast<UBalloon_GI>(UGameplayStatics::GetGameInstance(this));
		if (IsValid(gameInstance))
		{
			saveObj->playerLookSensitivity = gameInstance->getLookSensitivity();
			saveObj->highScore = gameInstance->getPlayerScore();
		}

		//checking if saveGame object already exists
		if (UGameplayStatics::DoesSaveGameExist(slotName, userIndex))
		{
			//Async loading
			FAsyncLoadGameFromSlotDelegate LoadedDelegate;
			LoadedDelegate.BindUObject(this, &ABalloon_GM::onAsyncLoadPlayInfo);
			UGameplayStatics::AsyncLoadGameFromSlot(slotName, userIndex, LoadedDelegate);
		}
		else
		{
			//storing in game mode current info
			gmHighScore = saveObj->highScore; //same as accessing game state->getplayerscore()
			gmHighestRound = roundNum;

			//current information already stored in saveObj above

			doAsyncSavePlay();
		}
	}
}

void ABalloon_GM::onAsyncLoadPlayInfo(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	UscoreSaver* existingSave = Cast<UscoreSaver>(LoadedGameData);
	if (IsValid(existingSave) && IsValid(saveObj))
	{
		int32 maxScore = FMath::Max<int32>(saveObj->highScore, existingSave->highScore);
		int32 maxRound = FMath::Max<int32>(saveObj->highestRound, existingSave->highestRound);
		saveObj->highScore = maxScore;
		saveObj->highestRound = maxRound;

		//storing in game mode
		gmHighScore = maxScore;
		gmHighestRound = maxRound;

		doAsyncSavePlay();
	}
}

void ABalloon_GM::doAsyncSavePlay()
{
	//performs async saving
	if (IsValid(saveObj))
	{
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		SavedDelegate.BindUObject(this, &ABalloon_GM::onAsyncSavePlayInfo);

		FString slotName = saveObj->saveSlotName;
		int32 userIndex = saveObj->userIndex;

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(saveObj, slotName, userIndex, SavedDelegate);
	}
}

void ABalloon_GM::onAsyncSavePlayInfo(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (!bSuccess)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Red, TEXT("CouldNotSaveInformation(HighScore)"));
}

void ABalloon_GM::saveGameStatus()
{
	/*https://docs.unrealengine.com/en-US/Gameplay/SaveGame/Code/index.html*/
	//statusObj already declared in header file! -> to allow access from other methods

	//creating saveGame object
	statusObj = Cast<UGameStatus>(UGameplayStatics::CreateSaveGameObject(UGameStatus::StaticClass()));
	if (IsValid(statusObj)) //makes sure reference is not null and is not destroyed
	{
		FString slotName = statusObj->saveSlotName;
		int32 userIndex = statusObj->userIndex;

		statusObj->roundNum = this->roundNum;
		statusObj->balProb = this->balProb;
		statusObj->spawnFrequency = this->spawnFrequency;

		//Using game instance to get right values
		UBalloon_GI* gameInstance = Cast<UBalloon_GI>(UGameplayStatics::GetGameInstance(this));
		if (IsValid(gameInstance))
		{
			statusObj->playerScore = gameInstance->getPlayerScore();
			statusObj->escapedBalloons = gameInstance->getEscapedBalloons();
		}


		int levelIndex = (roundNum - 1) / (maxRounds / numLevels);
		if (levelIndex < levelArray.Num())
			statusObj->currentLevel = levelArray[levelIndex];

		doAsyncSaveGameStatus();
	}
}

void ABalloon_GM::doAsyncSaveGameStatus()
{
	//performs async saving
	if (IsValid(statusObj))
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("Saving status info"));
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		SavedDelegate.BindUObject(this, &ABalloon_GM::onAsyncSaveGameStatus);

		FString slotName = statusObj->saveSlotName;
		int32 userIndex = statusObj->userIndex;

		//Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(statusObj, slotName, userIndex, SavedDelegate);
	}
}

void ABalloon_GM::onAsyncSaveGameStatus(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (!bSuccess)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Red, TEXT("CouldNotSaveInformation(GameStatus)"));

	//Does transition after status is saved
	if ((roundNum - 1) % (maxRounds / numLevels) == 0) //maxRounds/numLevels gets the number of rounds for each level
			//starts a level transition if this is the right round
		GetWorldTimerManager().SetTimer(startTransitionHandle, this, &ABalloon_GM::startLevelTransition, 1.f, false, transitionDelay);
	else
		startSpawnTimer(); //Continues balloon spawning
}

void ABalloon_GM::onAsyncLoadGameStatus(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	//UGameStatus onAsyncLoad implementation
	UGameStatus* existingStatus = Cast<UGameStatus>(LoadedGameData);
	if (IsValid(existingStatus))
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, TEXT("SaveGameLoads"));
		//setting up variables
		this->roundNum = existingStatus->roundNum;
		this->balProb = existingStatus->balProb;
		this->spawnFrequency = existingStatus->spawnFrequency;

		//Updates game instance
		UBalloon_GI* gameInstance = Cast<UBalloon_GI>(UGameplayStatics::GetGameInstance(this));
		if (IsValid(gameInstance))
		{
			gameInstance->setScore(existingStatus->playerScore);
			gameInstance->setRoundNum(roundNum);
			gameInstance->setEscapedBalloons(existingStatus->escapedBalloons);
		}

		//Implements calculations
		calculateRoundChars();
		
		//Starts timer after characteristics are calculated for accuracy
		if (bSpawnBalloons)
			startSpawnTimer();
	}
}

void ABalloon_GM::calculateRoundChars()
{
	//To make game easier; allows balloons escaped during one round to not harm the player too much in the later rounds
	maxEscapedBalloons = getNextMaxEscapedBalloons();
	UBalloon_GI* gameInstance = Cast<UBalloon_GI>(UGameplayStatics::GetGameInstance(this));
	if (IsValid(gameInstance))
	{
		gameInstance->setMaxEscapedBalloons(maxEscapedBalloons);
		gameInstance->setRoundNum(roundNum);
	}

	spawnFrequency = getNextFrequency();
	balLimit = getNextMax(spawnFrequency);
	numBalloonsLeft = balLimit; //Sets numBalloonsLeft to the total number of balloons to be spawned in the round
	spawnRadius = getNextSpawnRadius();

	//setNextProbVector(); //already done before calling this function
 
	//Increases refill rate
	AActor* pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ASlingshot* slingshot = Cast<ASlingshot>(pawn);
	if (slingshot != NULL)
		slingshot->increaseRefillRate(roundNum);

	if (bPrintRoundChars)
	{
		FString message = TEXT("Round ") + FString::FromInt(roundNum);
		message += TEXT("\nSpawn Frequency:: ") + FString::SanitizeFloat(spawnFrequency);
		message += TEXT("\nMax Balloons:: ") + FString::FromInt(balLimit);
		message += TEXT("\nSpawn Radius:: ") + FString::SanitizeFloat(spawnRadius);

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, message);
		//Source - https://wiki.unrealengine.com/String_Conversions:_FString_to_FName,_FString_to_Int32,_Float_to_FString
	}
}

int32 ABalloon_GM::getLevelIndex() const
{
	return (roundNum - 1) / (maxRounds / numLevels);
}