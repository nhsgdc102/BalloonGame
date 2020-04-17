// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "CP_BalloonMovementComponent.generated.h"

/**
 * CP_BalloonMovementComponent updates the position of another component during its tick.
 *
 * Simulated movement imitates that of a balloon moving upward through air.
 *
 * Normally the root component of the owning actor is moved, however another component may be selected (see SetUpdatedComponent()).
 * If the updated component is simulating physics, only the initial launch parameters (when initial velocity is non-zero)
 * will affect the projectile, and the physics sim will take over from there.
 *
 * @see UMovementComponent
 * Much of code is duplicated from ProjectileMovementComponent.h/.cpp
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity))
class BALLOONGAME_API UCP_BalloonMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UCP_BalloonMovementComponent(const FObjectInitializer& ObjectInitializer);

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// virtual void PostLoad() override;
	//End UActorComponent Interface

	//Begin UMovementComponent Interface
	virtual float GetMaxSpeed() const override { return TerminalVelocity; }
	virtual void InitializeComponent() override;
	// virtual void UpdateTickRegistration() override;
	//End UMovementComponent Interface

	bool HasStoppedSimulation() { return (UpdatedComponent == nullptr) || (IsActive() == false); }

private:
	// Adds up skin mass and helium mass
	float ComputeBalloonMass(const float InVolume) const;
	// Calculates mass after accounting for buoyancy
	float ComputeModifiedMass(const float InVolume) const;
	// Calculates terminal velocity based on drag, weight, buoyancy
	float ComputeTerminalVelocity(const float InArea) const;
	// Calculates the speed at the given time (assuming an initial speed of 0) (absolute time means the time since the balloon actor spawned)
	float ComputeSpeed(const float AbsoluteTime) const;
	// Returns the movement vector
	FVector ComputeMoveDelta(const FVector& InitVelocity, const FVector& FinalVelocity, const float DeltaTime) const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Largest diameter of balloon (m)
		float Diameter;
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Mass of plastic skin of balloon (kg)
		float SkinMass;
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Mass of string (kg)
		float StringMass;
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Density of helium inside balloon (kg/m^3)
		float HeliumDensity;
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Density of air outside balloon (kg/m^3)
		float AirDensity;
	UPROPERTY(EditDefaultsOnly, Category = "Buoyancy")
		// Gravity constant
		float GravConstant;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Force")
		// Numerical drag coefficient (typically = 0.40 for turbulent conditions)
		float DragCoeff;

	UPROPERTY(EditDefaultsOnly)
		float MetersToUnrealUnits;

private:
	/*Constants*/
	// Sum of skin mass and helium mass
	float BalloonMass;
	// Mass after accounting for buoyancy
	float ModifiedMass;
	// The max speed of the balloon
	float TerminalVelocity;
};
