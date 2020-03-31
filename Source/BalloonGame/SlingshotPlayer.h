// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
//For slingshot pawn
#include "Slingshot.h"
//For get world delta seconds
#include "Engine/World.h"

#include "SlingshotPlayer.generated.h"

/**
 *
 */
UCLASS()
class BALLOONGAME_API ASlingshotPlayer : public APlayerController
{
	GENERATED_BODY()

public:
	//Sets default values
	ASlingshotPlayer();

//Input
public:
	// Called every frame
	virtual void PlayerTick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

public:
	void StartPullingSlingshot();
	void EndPullingSlingshot();
	void PullSlingshot(float pullInput);

	void LookYaw(float yaw);
	void LookPitch(float pitch);

private:
	//Useful when bLockCameraOnPull is true
	bool canPlayerLookAround() const;

private:
	bool bIsPulling;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float pullSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float lookSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//-1 or 1
		float pullCorrectionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//-1 or 1
		float yawCorrectionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//-1 or 1
		float pitchCorrectionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//max rotation rate in degrees per second
		float turnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bLockCameraOnPull;

//User Widgets
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Widgets")
		bool bShowBalloonZones;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Widgets")
		bool bShowInfoDisplay;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "User Widgets")
		//Sets one zone to visible in Balloon Direction Widget--needs no C++ definition
		void DisplayBalloonZone(int32 zoneIndex);
};
