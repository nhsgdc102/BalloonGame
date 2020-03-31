// Fill out your copyright notice in the Description page of Project Settings.

#include "SlingshotPlayer.h"

ASlingshotPlayer::ASlingshotPlayer()
{
	bIsPulling = false;
	pullSensitivity = 61.f;
	lookSensitivity = 2.5f;
	pullCorrectionFactor = -1.f;
	yawCorrectionFactor = 1.f;
	pitchCorrectionFactor = -1.f;
	turnRate = 25.f;

	bLockCameraOnPull = false;

	bShowBalloonZones = true;
	bShowInfoDisplay = true;
}

// Called every frame
void ASlingshotPlayer::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ASlingshotPlayer::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Pull", this, &ASlingshotPlayer::PullSlingshot);

	InputComponent->BindAxis("Yaw", this, &ASlingshotPlayer::LookYaw);
	InputComponent->BindAxis("Pitch", this, &ASlingshotPlayer::LookPitch);
}

void ASlingshotPlayer::StartPullingSlingshot()
{
	if (!bIsPulling) {
		ASlingshot* slingshot = Cast<ASlingshot>(GetPawn());

		if (!slingshot->getHasRock()) //Only starts pulling when no rock mesh is present
		{
			bIsPulling = true;
			slingshot->startPulling();
		}
	}
}

void ASlingshotPlayer::EndPullingSlingshot()
{
	if (bIsPulling) {
		bIsPulling = false;

		ASlingshot* slingshot = Cast<ASlingshot>(GetPawn());
		slingshot->letGoOfRope(); //Lets go of rope and begins simulation if displacement meets threshold
	}
}

void ASlingshotPlayer::PullSlingshot(float pullInput)
{
	if (pullInput > 0.f) //joystick is below center
		StartPullingSlingshot();
	else //joystick is at or above center
		EndPullingSlingshot();

	if (bIsPulling) {
		float disp = pullInput * pullCorrectionFactor * pullSensitivity;

		//Setting displacement for slingshot
		ASlingshot* slingshot = Cast<ASlingshot>(GetPawn());
		slingshot->setDisplacement(disp, true); //true so that rope stretch sound can occur
	}
}

bool ASlingshotPlayer::canPlayerLookAround() const
{
	if (bLockCameraOnPull)
	{
		//cannot look around if slingshot has rock mesh present (either pulling or simulating going forward)
		//Prevents player from looking around and disrupting aim before rock is launched
		ASlingshot* slingshot = Cast<ASlingshot>(GetPawn());
		return !slingshot->getHasRock();
	}

	return true; //always true if no lock is on
}

void ASlingshotPlayer::LookYaw(float yaw)
{
	if (canPlayerLookAround())
	{
		float maxTurnAngle = turnRate * GetWorld()->GetDeltaSeconds(); //to keep rotation constant in varying frame rates
		AddYawInput(yaw*yawCorrectionFactor*lookSensitivity*maxTurnAngle);
	}
}

void ASlingshotPlayer::LookPitch(float pitch)
{
	if (canPlayerLookAround())
	{
		float maxTurnAngle = turnRate * GetWorld()->GetDeltaSeconds(); //to keep rotation constant in varying frame rates
		AddPitchInput(pitch*pitchCorrectionFactor*lookSensitivity*maxTurnAngle);
	}
}

