// Fill out your copyright notice in the Description page of Project Settings.

#include "Slingshot.h"

FName ASlingshot::rootName(TEXT("Root"));
FName ASlingshot::cameraName(TEXT("Camera"));
FName ASlingshot::frameName(TEXT("Frame"));
FName ASlingshot::holderName(TEXT("Holder"));

// Sets default values
ASlingshot::ASlingshot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(ASlingshot::rootName);
	camera = CreateDefaultSubobject<UCameraComponent>(ASlingshot::cameraName);
	frame = CreateDefaultSubobject<UStaticMeshComponent>(ASlingshot::frameName);
	holder = CreateDefaultSubobject<UStaticMeshComponent>(ASlingshot::holderName);
	RootComponent = root;
	camera->SetupAttachment(RootComponent);
	frame->SetupAttachment(RootComponent);
	holder->SetupAttachment(RootComponent);

	//Setting up camera
	camera->SetRelativeLocation(FVector(-77.5f, 0.f, 12.f)); //May be an incorrect way to set relative location

	//Setting up frame
	frame->SetRelativeLocation(FVector(2.8f, 0.f, -12.5f));
	frame->SetGenerateOverlapEvents(true);
	frame->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; //No need for stepping on slingshot frame
	frame->SetCollisionProfileName("Slingshot");

	//Setting up holder
	holder->SetGenerateOverlapEvents(true);
	holder->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; //No need for stepping on slingshot holder
	holder->SetCollisionProfileName("Slingshot");

	displacement = 0.f;
	timePassed = 0.f;
	bIsSimulating = false;
	bHasRock = false;

	minimumDisplacement = -61.f;
	minDispCoeff = -0.75f;
	rockVelocity = FVector();
	stretchSoundTolerance = 0.1f;
	volMult_StretchSound = 1.f;
	bIsStretchSoundPaused = false;
	volMult_LaunchSound = 1.f;

	mass = 0.15f;
	elastConst = 300.f;
	unit = 50.f;
	k1 = 1.25f;
	k3 = 0.5f;
	k4 = 0.75f;
	initDisp = 0.f;
	restCoeff = -0.1f;

	numDivisions = 10;
	initRefillRate = 0.04f;
	refillRate = 0.04f;
	refillRateInc = 0.0064f;
	refillPercentage = 1.f;

	//Rotation Calibration
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;

	bPrintSimDisplacement = true;
	bPrintIntValues = true;
	bPrintRefillPercentage = true;
	bPrintAllDisplacement = true;
	bPrintSlingshotStages = true;
}

// Called when the game starts or when spawned
void ASlingshot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASlingshot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Updating cooldown/refill bar
	setRefillPercentage(refillRate*DeltaTime + refillPercentage);
	if (bPrintRefillPercentage)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Black,
			TEXT("Slingshot Refill: " +
				FString::SanitizeFloat(100.f*refillPercentage) + "%"));

	//Simulation
	if (bIsSimulating)
		simulateSlingshot(DeltaTime);

	if (bPrintAllDisplacement)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Black,
			TEXT("Slingshot Disp: " +
				FString::SanitizeFloat(displacement)));
}

// Called to bind functionality to input
void ASlingshot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASlingshot::setDisplacement(float disp, bool bPlayerPulling)
{
	//Sets displacement in reasonable range
	disp = FMath::Clamp<float>(disp, minimumDisplacement, 0.f);
	float delta = disp - displacement; //used for deciding to play stretching sound
	displacement = disp; //assigns new disp to displacement

	FQuat cameraRot = camera->GetRelativeTransform().GetRotation();
	FVector newLoc = displacement * cameraRot.GetForwardVector(); //Because displacement is negative, result will be backward

	holder->SetRelativeLocation(newLoc);

	if (bPlayerPulling) { //Makes it so that the stretch sound cannot play while the holder is going forward (during simulation)
		if (fabsf(delta) >= stretchSoundTolerance) { //Only executes if the player moves slingshot enough, prevents constant starts and stops
			playStretchSound(); //Will either start playing sound or resume it
		}
		else {
			stopStretchSound();
		}
	}
}

bool ASlingshot::slingRock()
{
	if (bPrintSlingshotStages)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Slingshot: running slingRock"));


	bool validDisp = displacement <= minDispCoeff * unit; //Only displacements above (or below) a certain threshold can be simulated without errors
	if (validDisp && bHasRock) { //Should only work if rock mesh is present
		//Setting up initial values
		initDisp = displacement;
		timePassed = 0.f;
		bIsSimulating = true;

		setUpIntermediateValues();
		return true; //rock is now successfully being launched
	}
	else {
		if (IsValid(staticRock)) //Prevents error message
			staticRock->Destroy();

		bHasRock = false; //In case rock mesh was present but displacement did not meet threshold

		return false; //rock is not being launched
	}

	return false; //default return
}

bool ASlingshot::spawnRock()
{
	if (bPrintSlingshotStages)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Slingshot: running spawnRock"));

	rockVelocity = calcVelocity(); //Used to set velocity in spawned rock
	FTransform spawnTransform = staticRock->GetActorTransform();
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this; //Setting owner as slingshot object

	AMovableRock* launchedRock = GetWorld()->SpawnActor<AMovableRock>(projectileRockType, spawnTransform, spawnParams);
	if (IsValid(staticRock)) //Prevents error message
		staticRock->Destroy(); //Destroys attached rock

	//Plays sound of rock being launched (occurs whether or not it actually spawned)
	playLaunchRockSound();

	bHasRock = false; //must occur for another rock to be able to be thrown

	//Gives rock correct velocity
	if (IsValid(launchedRock)) //only successful if spawn occured successfully
	{
		launchedRock->setRockVelocity(rockVelocity);
		return true;
	}

	return false; //launchedRock does not exist, so spawn was unsuccessful
}

bool ASlingshot::attachRock()
{
	if (bPrintSlingshotStages)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Slingshot: running attachRock"));

	if (!bHasRock && refillPercentage >= (1.f / numDivisions)) { //Only occurs if rock is absent previously and there is enough meter to fire another rock
		//1.f / numDivisions gets the amount of meter needed for one rock

		//Copied from KismetMathLibrary.cpp->FRotator UKismetMathLibrary::RandomRotator(bool bRoll)
		FRotator randRotator;
		randRotator.Yaw = FMath::FRand() * 360.f;
		randRotator.Pitch = FMath::FRand() * 360.f;
		randRotator.Roll = FMath::FRand() * 360.f;

		FTransform spawnTransform = FTransform(randRotator);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //rock should have no collision anyway

		staticRock = Cast<AStaticMeshActor>(GetWorld()->SpawnActor(staticRockType, &spawnTransform, spawnParams));

		if (IsValid(staticRock)) //only attaches rock if spawn occurred successfully
		{
			//Snap to target makes sure rock's transform is editable through socket's transform, weldsimulatedbodies is false because rock needs no physics simulation
			staticRock->AttachToComponent(holder, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), TEXT("rockSocket"));
			bHasRock = true;

			return true; //rock did spawn successfully
		}

		return false; //rock did not spawn successfully
	}

	return false; //cannot spawn rock
}

bool ASlingshot::getHasRock() const
{
	return bHasRock;
}

void ASlingshot::startPulling()
{
	if (bPrintSlingshotStages)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Slingshot: running startPulling"));

	bIsSimulating = false; //Ensures that slingshot does not continue simulating while player is pulling back
	attachRock();
}

void ASlingshot::letGoOfRope()
{
	if (bPrintSlingshotStages)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Slingshot: running letGoOfRope"));

	bool success = slingRock();
	if (!success) //if rock did not spawn or displacement did not meet threshold
	{
		setDisplacement(0.f, false); //resets slingshot
	}

	//Ensures that stretching sound stops
	if (IsValid(stretchSoundComp))
		stretchSoundComp->Stop();
}

FVector ASlingshot::getRockVelocity() const
{
	return rockVelocity;
}

//To be overidden in blueprints
void ASlingshot::playLaunchRockSound_Implementation() const
{
}

void ASlingshot::playStretchSound()
{
	if (IsValid(stretchSoundComp)) { //if it has already been created
		if (bIsStretchSoundPaused) { //only will play sound if it has been paused previously
			stretchSoundComp->SetPaused(false);
			bIsStretchSoundPaused = false;
		}
	}
	else {
		stretchSoundComp = spawnStretchSound(); //easier to spawn from blueprints
		bIsStretchSoundPaused = false;
	}
}

void ASlingshot::stopStretchSound()
{
	bIsStretchSoundPaused = pauseStretchSound(); //true if it paused successfully
}

//To be overidden in blueprints
UAudioComponent* ASlingshot::spawnStretchSound_Implementation() const
{
	UAudioComponent* x = NULL;
	return x; //must return something
}

//To be overidden in blueprints
bool ASlingshot::pauseStretchSound_Implementation() const
{
	return false;
}

void ASlingshot::setUpIntermediateValues()
{
	//Stage 1
	initVel = sqrtf(elastConst*powf(initDisp, 2.f) / mass);
	t1 = (PI / 2.f)*(-1.f*initDisp / initVel);
	//Stage 2
	w1 = (-1.f*initVel / initDisp)*k1;
	t2 = -1.f*(asinf(3.f*unit / (4.f*initDisp)) / w1) + t1;
	//Stage 3
	w2 = w1 * k3;
	deltaT = PI / (2.f*w1) - asinf(3.f*unit / (4.f*initDisp)) / w1;
	k2 = sinf(w1*(t2 - t1)) / cosf(w2*deltaT);
	t3 = PI / (2.f*w2) - deltaT + t2;
	//Stage 4
	w3 = w2 * k4;
	t4 = PI / (2.f*w3) + t3;

	if (bPrintIntValues)
	{
		FString message = "Intermediate Slingshot Values:";
		message +=
			"\nrelease velocity = " + FString::SanitizeFloat(initVel) +
			"\nt1 = " + FString::SanitizeFloat(t1) +
			"\nt2 = " + FString::SanitizeFloat(t2) +
			"\nt3 = " + FString::SanitizeFloat(t3) +
			"\nt4 = " + FString::SanitizeFloat(t4) +
			"\ndeltaT = " + FString::SanitizeFloat(deltaT) +
			"\nw1 = " + FString::SanitizeFloat(w1) +
			"\nw2 = " + FString::SanitizeFloat(w2) +
			"\nw3 = " + FString::SanitizeFloat(w3) +
			"\nk2 = " + FString::SanitizeFloat(k2);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Black, message);
	}
}

float ASlingshot::timeToDisplacement(float time)
{
	float disp = 0.f; //displacement

	//Stage 1
	if (time <= t1)
		disp = initDisp * cosf((-1.f*initVel / initDisp)*time);
	//Stage 2
	else if (time <= t2)
		disp = -1.f*initDisp*sinf(w1*(time - t1));
	//Stage 3
	else if (time <= t3)
		disp = -1.f*k2*initDisp*cosf(w2*(time - t2 + deltaT));
	//Stage 4
	else if (time <= t4)
		disp = restCoeff * unit*sinf(w3*(time - t3));
	//Stage 5
	else
		disp = restCoeff * unit;

	if (bPrintSimDisplacement)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black,
			"Time " + FString::SanitizeFloat(time) +
			" -> Displacement " + FString::SanitizeFloat(disp));

	return disp;
}

void ASlingshot::setIsSimulating(bool sim)
{
	bIsSimulating = sim;
}

void ASlingshot::simulateSlingshot(float deltaTime)
{
	timePassed += deltaTime;
	setDisplacement(timeToDisplacement(timePassed), false); //timeToDisplacement retrieves correct displacement at current time

	if (displacement >= 0.f && bHasRock) { //slingshot has reached release point and rock mesh is present
		bool success = spawnRock();
		if (success) //Does not affect meter if no rock was launched
			setRefillPercentage(refillPercentage - (1.f / numDivisions)); //1.f / numDivisions gets the refill percentage for one slot
	}
}

FVector ASlingshot::calcVelocity()
{
	float velFactor = sqrtf(powf(initDisp, 2.f)*elastConst / mass); //Uses spring potential energy formula
	//FQuat cameraRot = camera->GetRelativeTransform().GetRotation(); //used for camera's forward vector
	FQuat cameraRot = camera->GetComponentTransform().GetRotation(); //used for camera's forward vector -- world rotation changes while relative rotation is constant
	return velFactor * cameraRot.GetForwardVector();
}

void ASlingshot::setRefillPercentage(float result)
{
	refillPercentage = FMath::Clamp<float>(result, 0.f, 1.f);
}

float ASlingshot::getRefillPercentage() const
{
	return refillPercentage;
}

void ASlingshot::increaseRefillRate(int32 roundNum)
{
	//Linear explicit function
	refillRate = initRefillRate + refillRateInc * (roundNum - 1);

	//Recursive
	//refillRate += refillRateInc;
}

