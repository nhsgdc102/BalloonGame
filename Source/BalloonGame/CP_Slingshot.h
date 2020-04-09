// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CP_Slingshot.generated.h"

/*Forward Declarations*/
class UCameraComponent; //Other component classes are already included in parent class header files

UCLASS(Abstract)
class BALLOONGAME_API ACP_Slingshot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACP_Slingshot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Binded to player input to change how far back slingshot is pulled
	void PullSlingshot(float Val);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/*Component Declarations*/
private:
	UPROPERTY(Category = "Slingshot", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		// The entire slingshot, including the y-shaped frame, the elastic cords, and the holder for the rock
		USkeletalMeshComponent* MeshComp;
	UPROPERTY(Category = "Slingshot", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		// Perspective from which the player views the game
		UCameraComponent* CameraComp;


};
