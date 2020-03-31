// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlingshotPlayer.h" //For camera interaction
#include "Kismet/KismetMathLibrary.h" //For vector calculations
#include "Kismet/GameplayStatics.h" //For popping sound
#include "Sound/SoundBase.h" //For popping sound
#include "Materials/MaterialInstanceDynamic.h" //For popping dynamic material
#include "UObject/ConstructorHelpers.h" //For getting balloon material
#include "TimerManager.h" //For setting timer before death and setting loop for material
#include "Engine/Engine.h"

#include "Balloon.generated.h"

class UTimerManager;

UCLASS()
class BALLOONGAME_API ABalloon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABalloon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when actor is removed from level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//Balloon Camera Interaction
public:
	UFUNCTION(BlueprintCallable)
		//Returns -1 if on screen
		int getDirectionFromCamera(APlayerController* player);

private:
	float getAngleBetweenVectors(FVector up, FVector right, FVector w);

	bool isOffScreen(APlayerController* player, FVector actorLoc);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BalloonAndCamera")
		bool bInteractWithCamera;

//Sound
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		float amplificationConst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* poppingSound;

public:
	UFUNCTION(BlueprintCallable)
		void playPopSound();

//Dynamic popping material instance
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
		UMaterial* BalloonMaterial;
	UPROPERTY(BlueprintReadWrite, Category = "Material")
		UMaterialInstanceDynamic* DynamicPopInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
		float popSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
		float updatePopInterval;
	//Access to timer that loops and updates popping animation
	FTimerHandle UpdatePopHandle;

	//Binded to destroy self--used to have a delay before death
	FTimerHandle DeathTimerHandle;

public:
	UFUNCTION()
		void UpdatePopAnimation();

private:
	/*
	bool bIsPopping;
	*/
	float popTime;

//Miscellaneous
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		bool bDead;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		//Deprecated--useless now
		FVector rockVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Miscellaneous")
		int32 scoreAddition;

public:
	UFUNCTION(BlueprintCallable)
		void callDeath(bool bPlaySound);
	void DestroyBalloon();

//Debugging
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintOnScreen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bDebugCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bDebugMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bPrintAmplification;
};
