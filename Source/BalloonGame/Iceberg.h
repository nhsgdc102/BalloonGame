// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameFramework/RotatingMovementComponent.h"
#include "TimerManager.h"

#include "Iceberg.generated.h"

class URotatingMovementComponent;
class UTimerManager;

UCLASS()
class BALLOONGAME_API AIceberg : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIceberg();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

/*
	// Called when actor is removed from level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//Rotation Variables
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		//Time in each cycle of the rotation rate
		float period;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		//Maximum rotation rate
		float amplitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		//rotation constant--must set in construction script if changing period in blueprints
		float w;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
		//Time between updates to rotation--keep above 0.016 or 1/60
		float updateInterval;

private:
	//Keeps track of current time in cycle
	float timePassed;

/*
//Timer variables
public:
	FTimerHandle RotationHandle;
*/

//Rotation-related functions
/*
public:
	UFUNCTION()
		void UpdateRotation();
*/

private:
	float getRotationRate(float currentTime) const;

//Rotating component
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		URotatingMovementComponent* rotationComp;
	static FName RotatingMovementCompName;
};
