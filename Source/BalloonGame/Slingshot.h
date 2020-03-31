// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
//For frame and holder components
#include "Components/StaticMeshComponent.h"
//For static rock
#include "Engine/StaticMeshActor.h"
//For sounds
#include "Sound/SoundBase.h"
//For storing created stretch sound
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
//For launching rock
#include "MovableRock.h"

#include "Slingshot.generated.h"

UCLASS()
class BALLOONGAME_API ASlingshot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASlingshot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Components
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		//specifying a separate root component ensures camera does not become root
		USceneComponent* root;
	static FName rootName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* camera;
	static FName cameraName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* frame;
	static FName frameName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* holder;
	static FName holderName;

	//Pulling back slingshot
private:
	//current displacement of slingshot
	float displacement;
	//Used for simulating slingshot
	float timePassed;
	//whether or not simulating
	bool bIsSimulating;
	//whether or not has rock
	bool bHasRock;
	//To be accessed by rock upon launching
	FVector rockVelocity;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		float minimumDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		float minDispCoeff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		TSubclassOf<AStaticMeshActor> staticRockType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		TSubclassOf<AMovableRock> projectileRockType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Sound of rope stretching
		USoundBase* stretchSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Minimum change in displacement for stretching sound to play
		float stretchSoundTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Stores created audio component for rope stretching
		UAudioComponent* stretchSoundComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Volume multiplier
		float volMult_StretchSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Whether or not sound is paused
		bool bIsStretchSoundPaused;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		//Sound of rock launching
		USoundBase* rockLaunchSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PullingSlingshot")
		float volMult_LaunchSound;

private:
	//Temporarily spawned to be attached to the slingshot
	AStaticMeshActor* staticRock;

public:
	void setDisplacement(float disp, bool bPlayerPulling);
	bool slingRock();
	//boolean tells whether or not func executed successfully
	bool spawnRock();
	//boolean tells whether or not func executed successfully
	bool attachRock();

	void playStretchSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PullingSlingshot")
		UAudioComponent* spawnStretchSound() const;
	//allows for modifying object in c++ and then adding other functionality in blueprints
	void stopStretchSound();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PullingSlingshot")
		bool pauseStretchSound() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PullingSlingshot")
		void playLaunchRockSound() const;
	//source - https://wiki.unrealengine.com/Blueprints,_Empower_Your_Entire_Team_With_BlueprintNativeEvents

	UFUNCTION(BlueprintCallable)
		//Returns whether or not it has a rock mesh
		bool getHasRock() const;
	UFUNCTION(BlueprintCallable)
		//Called to attach rock and begin pulling back
		void startPulling();
	UFUNCTION(BlueprintCallable)
		//Called to sling rock or to reset slingshot
		void letGoOfRope();

	UFUNCTION(BlueprintPure)
		FVector getRockVelocity() const;


	//Slingshot simulation
public:
	//Controllable variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//mass of rock; higher mass->slower release velocity
		float mass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//elastic constant of slingshot
		float elastConst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//initial displacement
		float initDisp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//conversion between in-game units and units for displacement animation
		float unit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//higher than 1
		float k1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//less than 1
		float k3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//less than 1
		float k4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
		//resting displacement of slingshot
		float restCoeff;

private:
	//Intermediate calculations
	//release velocity of projectile
	float initVel;
	//last value of time for interval of stage 1
	float t1;
	//last value of time for interval of stage 2
	float t2;
	//last value of time for interval of stage 3
	float t3;
	//last value of time for interval of stage 4
	float t4;
	//time left for stage 2 curve to hit peak
	float deltaT;
	//influences stage 1 curve
	float w1;
	//influences stage 2 curve
	float w2;
	//influences stage 3 curve
	float w3;
	//makes sure stage 2 curve is equal to stage 3 at t2 (continuous)
	float k2;

public:
	UFUNCTION(BlueprintCallable)
		//Sets up intermediate values
		void setUpIntermediateValues();
	UFUNCTION(BlueprintCallable)
		//Displacement function
		float timeToDisplacement(float time);
	UFUNCTION(BlueprintCallable)
		void setIsSimulating(bool sim);

private:
	void simulateSlingshot(float deltaTime);
	FVector calcVelocity();

	//Slingshot cooldown/refill
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		int numDivisions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		float initRefillRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		float refillRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		float refillRateInc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		//Default should be 1
		float refillPercentage;

public:
	UFUNCTION(BlueprintCallable)
		//Appropriately sets refill percentage
		void setRefillPercentage(float result);
	UFUNCTION(BlueprintPure)
		float getRefillPercentage() const;

	//To be called by game mode on round transitions
	void increaseRefillRate(int32 roundNum);

	//Debugging
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		//print intermediate values
		bool bPrintIntValues;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintSimDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintRefillPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintAllDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		//prints stages of slinging rock
		bool bPrintSlingshotStages;
};
