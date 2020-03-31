// Copyright 2020 Northview High School Game Development Club


#include "Iceberg.h"

FName AIceberg::RotatingMovementCompName(TEXT("Rotating Movement"));

// Sets default values
AIceberg::AIceberg()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = updateInterval;

	//Setting up rotation component
	rotationComp = CreateDefaultSubobject<URotatingMovementComponent>(AIceberg::RotatingMovementCompName);
	rotationComp->RotationRate = FRotator(0.f);

	//Default values
	period = 10.f;
	amplitude = 0.5f;
	updateInterval = 0.15f;

	timePassed = 0.f;
	w = 2.f*PI / period;
}

// Called when the game starts or when spawned
void AIceberg::BeginPlay()
{
	Super::BeginPlay();
	
	/*
	//Starts timer loop
	GetWorldTimerManager().SetTimer<AIceberg>(RotationHandle, this, &AIceberg::UpdateRotation, updateInterval, true);
	*/
}

/*
// Called when actor is removed from level
void AIceberg::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Prevents loop from continuing after this actor is gone
	GetWorldTimerManager().ClearTimer(RotationHandle);
}
*/

// Called every frame
void AIceberg::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Updating rotation
	timePassed = FMath::Fmod(timePassed + DeltaTime, period);
	float rotRate = getRotationRate(timePassed);
	rotationComp->RotationRate = FRotator(rotRate, 0.f, rotRate);
}

/*
void AIceberg::UpdateRotation()
{
	timePassed = FMath::Fmod(timePassed + updateInterval, period);
	float rotRate = getRotationRate(timePassed);
	rotationComp->RotationRate = FRotator(rotRate, 0.f, rotRate);
}
*/

float AIceberg::getRotationRate(float currentTime) const
{
	//Simple Harmonic Motion--calculating rate of change of angle theta over time
	return amplitude * cosf(currentTime*w);
}

