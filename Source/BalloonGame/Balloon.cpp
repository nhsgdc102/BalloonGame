// Fill out your copyright notice in the Description page of Project Settings.

#include "Balloon.h"

// Sets default values
ABalloon::ABalloon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bInteractWithCamera = true;

	amplificationConst = 0.00025f;

	bDead = false;
	rockVelocity = FVector(0.f);
	scoreAddition = 0;

	/*
	//Setting up Dynamic Balloon Material Instance
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BalloonMaterial(TEXT("/Game/Actors/ActorMaterials/balloon_Mat"));
	/*Source - https://docs.unrealengine.com/en-US/Programming/Assets/ReferencingAssets/index.html */
	//BalloonMaterial.Object will be equal to nullptr if file finding failed, this will crash program if not checked for
	//

	//DynamicPopInstance = UMaterialInstanceDynamic::Create(BalloonMaterial, NULL); //Not sure what second parameter does
	
	/*
	bIsPopping = false;
	*/
	popTime = 0.f;
	popSpeed = 10.f;
	updatePopInterval = 0.15f;

	//Debugging
	bPrintAngle = false;
	bPrintOnScreen = false;
	bPrintAmplification = false;
	bDebugMode = false;
	bDebugCollision = false;
}

// Called when the game starts or when spawned
void ABalloon::BeginPlay()
{
	Super::BeginPlay();

}

// Called when actor is removed from level
void ABalloon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Prevents loop from continuing after this actor is gone
	GetWorldTimerManager().ClearTimer(UpdatePopHandle);
}

// Called every frame
void ABalloon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bInteractWithCamera)
	{
		ASlingshotPlayer* player = Cast<ASlingshotPlayer>(UGameplayStatics::GetPlayerController(this, 0));
		if (IsValid(player))
		{
			int zoneIndex = getDirectionFromCamera(player);
			if (zoneIndex > -1)
				player->DisplayBalloonZone(zoneIndex);
		}
	}

	/*
	if (bIsPopping)
	{
		popTime += DeltaTime;
		if (IsValid(DynamicPopInstance))
			DynamicPopInstance->SetScalarParameterValue(TEXT("Pop_Time"),
				FMath::Clamp<float>(popTime*popSpeed, 0.f, 1.f)); //Clamp prevents scalar from going over 1 and causing the pop animation to loop
		else
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Not working"));
	}
	*/
}

int ABalloon::getDirectionFromCamera(APlayerController* player)
{
	bool bOffScreen = isOffScreen(player, this->GetActorLocation());

	if (bPrintOnScreen)
	{
		if (bOffScreen)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black,
				TEXT("Balloon::OnScreen->false"));
		else
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black,
				TEXT("Balloon::OnScreen->true"));
	}

	if (bOffScreen)
	{
		FRotator camRot = player->PlayerCameraManager->GetCameraRotation();
		//Copied from KismetMathLibrary.cpp
		FVector upVector = FRotationMatrix(camRot).GetScaledAxis(EAxis::Z);
		FVector rightVector = FRotationMatrix(camRot).GetScaledAxis(EAxis::Y);
		FVector fwdVector = camRot.Vector();

		FVector wVector; //Vector between offset and its projeciton
		FVector offset = GetActorLocation() -
			(player->PlayerCameraManager->GetCameraLocation());

		wVector = offset - offset.ProjectOnTo(fwdVector);

		float angle = getAngleBetweenVectors(upVector, rightVector, wVector);

		if (bPrintAngle)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black,
				TEXT("Balloon::Angle = ") + FString::SanitizeFloat(angle));

		int zone = 0; //each zone is a range of angles
		if (angle <= 22.5f)
			zone = 0;
		else if (angle <= 67.5f)
			zone = 1;
		else if (angle <= 112.5f)
			zone = 2;
		else if (angle <= 157.5)
			zone = 3;
		else if (angle <= 202.5f)
			zone = 4;
		else if (angle <= 247.5f)
			zone = 5;
		else if (angle <= 292.5f)
			zone = 6;
		else if (angle <= 337.5f)
			zone = 7;
		//If angle is above 337.5 zone will be 0

		return zone;
	}

	return -1;
}

float ABalloon::getAngleBetweenVectors(FVector up, FVector right, FVector w) //w is vector between balloon offset and its projection
{
	float coeff; //equal to 1 when w is on left side, equal to 0 when w is on right side
	float angle;
	FVector horizontal;

	//Finds whether or not vector w is on right or left side of vector up
	horizontal = w - w.ProjectOnTo(up); //Produces vector either directed left or right
	horizontal = horizontal.GetSafeNormal(); //If the vector is too small this will not work
	if (horizontal.Equals(right, 0.0001f))
		coeff = 0.f;
	else
		coeff = 1.f;

	//Calculates Angle
	w = w.GetSafeNormal();
	angle = acosf(FVector::DotProduct(w, up));
	angle *= (180.f / PI); //Conversion to degrees from radians
	angle = FMath::Abs((coeff*360.f) - angle);
	return angle;
}

bool ABalloon::isOffScreen(APlayerController * player, FVector actorLoc)
{
	FVector2D ScreenLoc = FVector2D();
	FVector2D& ScreenLocPtr = ScreenLoc;
	int32 sizeX = 0;
	int32& sizeXPtr = sizeX; //Must store reference in other variable or there is an error
	int32 sizeY = 0;
	int32& sizeYPtr = sizeY;

	if (player->ProjectWorldLocationToScreen(actorLoc, ScreenLocPtr)) //Only runs if it is reportedly on screen //Meant to double check result
	{
		player->GetViewportSize(sizeXPtr, sizeYPtr);
		//When balloon is off screen, its position is either negative or above size limit
		bool offX = ScreenLocPtr.X < 0.f || ScreenLocPtr.X >(float) sizeXPtr;
		bool offY = ScreenLocPtr.Y < 0.f || ScreenLocPtr.Y >(float) sizeYPtr;

		return offX || offY;
	}

	return true; //runs if balloon is way off screen
}

void ABalloon::playPopSound()
{
	if (!(poppingSound == NULL))
	{
		FVector loc = this->GetActorLocation();
		/*
		float rockSpeed = FMath::Abs<float>(rockVelocity.Size()); //sets rockSpeed equal to vector length

		float volMultiplier = FMath::Clamp<float>(rockSpeed*amplificationConst, 0.f, 1.f); //volumeMultiplier should not exceed 1
		if (bPrintAmplification)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::Black, TEXT("Amplification: " + FString::SanitizeFloat(volMultiplier)));
		*/
		float volMultiplier = 1.f;

		float pitchMultiplier = FMath::RandRange(0.75f, 1.25f);
		UGameplayStatics::PlaySoundAtLocation(this, poppingSound, loc, volMultiplier, pitchMultiplier);
	}
}

void ABalloon::UpdatePopAnimation()
{
	popTime += updatePopInterval;
	if (IsValid(DynamicPopInstance))
		DynamicPopInstance->SetScalarParameterValue(TEXT("Pop_Time"),
			FMath::Clamp<float>(popTime*popSpeed, 0.f, 1.f)); //Clamp prevents scalar from going over 1 and causing the pop animation to loop
	else
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("Not working"));
}

void ABalloon::callDeath(bool bPlaySound)
{
	if (bPlaySound) //bPlaySound indicates whether or not balloon died to rock, which means that it popped and needs the popping sound and animation
	{
		playPopSound();
		//Starts loop to update popping animation
		GetWorldTimerManager().SetTimer<ABalloon>(UpdatePopHandle, this, &ABalloon::UpdatePopAnimation, updatePopInterval, true);
		/*
		bIsPopping = true;
		*/
	}
	
	bDead = true; //stops other functions from occuring before balloon dies
	
	//Calls timer before death
	GetWorldTimerManager().SetTimer<ABalloon>(DeathTimerHandle, this, &ABalloon::DestroyBalloon, 1.f / popSpeed, false);
}

void ABalloon::DestroyBalloon()
{
	//This is used because the function in the set timer parameters must return no value, and Destroy returns a boolean
	Destroy();
}