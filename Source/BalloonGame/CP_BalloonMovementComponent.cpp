// Copyright 2020 Northview High School Game Development Club


#include "CP_BalloonMovementComponent.h"
#include "GameFramework/Actor.h"

// Constructor
UCP_BalloonMovementComponent::UCP_BalloonMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Diameter = 0.381f;
	SkinMass = 0.0013f;
	StringMass = 0.02f;
	HeliumDensity = 0.169f;
	AirDensity = 1.225f;
	DragCoeff = 0.40f;
	GravConstant = 9.81f;

	MetersToUnrealUnits = 100.f;

	Velocity = FVector(0.f);
}

// Called when component is initialized
void UCP_BalloonMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Conversion to Unreal Units
	Diameter *= MetersToUnrealUnits;
	HeliumDensity /= powf(MetersToUnrealUnits, 3.f);
	AirDensity /= powf(MetersToUnrealUnits, 3.f);
	GravConstant *= MetersToUnrealUnits;

	// Sets constants beforhand for use in calculations
	const float Radius = Diameter / 2.f;
	const float Volume = 4.f*PI*powf(Radius, 3.f) / 3.f;
	const float CrossSectionArea = PI * powf(Radius, 2.f);
	BalloonMass = ComputeBalloonMass(Volume);
	ModifiedMass = ComputeModifiedMass(Volume); // Uses balloon mass in calculation!
	TerminalVelocity = ComputeTerminalVelocity(CrossSectionArea); // Uses modified mass in calculation!
}

// Adds up skin mass and helium mass
float UCP_BalloonMovementComponent::ComputeBalloonMass(const float InVolume) const
{
	/**
	* m_balloon = (p_helium)*V + m_skin + m_string,
	* where V is volume, m_skin is mass of balloon skin, m_string is mass of string
	*/
	return HeliumDensity * InVolume + SkinMass + StringMass;
}

// Calculates mass after accounting for buoyancy
float UCP_BalloonMovementComponent::ComputeModifiedMass(const float InVolume) const
{
	/**
	* M' = p_air*V - m_balloon
	*/
	const float ModifiedMass = AirDensity * InVolume - BalloonMass;
	if (ModifiedMass <= 0.f)
		UE_LOG(LogTemp, Warning, TEXT("UCP_BalloonMovementComp->Modified Mass <= 0!"));
	return ModifiedMass;
}

// Calculates terminal velocity based on drag, weight, buoyancy
float UCP_BalloonMovementComponent::ComputeTerminalVelocity(const float InArea) const
{
	/**
	* v_ter = sqrt(2M'g/(CAp_air)), where p_air is air density
	*/
	return sqrtf(
		2.f*ModifiedMass*GravConstant /
		(DragCoeff*InArea*AirDensity)
	);
}

// Called every tick
void UCP_BalloonMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(UpdatedComponent) /*|| !bSimulationEnabled*/)
	{
		return;
	}

	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!IsValid(ActorOwner) /*|| !CheckStillInWorld()*/)
	{
		return;
	}

	if (UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	/*Movement*/
	FVector MoveDelta;
	// Checks if terminal speed is reached and avoids computing new velocity if so
	if (FMath::IsNearlyEqual(Velocity.Size(), TerminalVelocity))
	{
		MoveDelta = ComputeMoveDelta(Velocity, Velocity, DeltaTime);
	}
	else
	{
		const FVector OldVelocity = Velocity;
		Velocity = FVector(0.f, 0.f, ComputeSpeed(ActorOwner->GetGameTimeSinceCreation())); // New velocity
		MoveDelta = ComputeMoveDelta(OldVelocity, Velocity, DeltaTime);
	}

	FHitResult Hit(1.f); // stores hit result from movement

	// Move the component
	// If we can't bounce, then we shouldn't adjust if initially penetrating, because that should be a blocking hit that causes a hit event and stop simulation.
	TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags, MoveComponentFlags | MOVECOMP_NeverIgnoreBlockingOverlaps);
	MoveUpdatedComponent(MoveDelta, UpdatedComponent->GetComponentQuat(), true, &Hit); // sweep movement allows collision

	// If we hit a trigger that destroyed us, abort.
	if (ActorOwner->IsPendingKill() || HasStoppedSimulation())
	{
		return;
	}

	UpdateComponentVelocity();
}

// Calculates the speed at the given time (assuming an intitial speed of 0) (absolute time means the time since the balloon actor spawned)
float UCP_BalloonMovementComponent::ComputeSpeed(const float AbsoluteTime) const
{
	/**
	* v = v_ter*tanh(M'gt/(m_balloon*v_ter))
	*/
	return TerminalVelocity * tanhf(
		ModifiedMass*GravConstant*AbsoluteTime/
		(BalloonMass*TerminalVelocity)
	);
}

// Returns the movement vector
FVector UCP_BalloonMovementComponent::ComputeMoveDelta(const FVector& InitVelocity, const FVector& FinalVelocity, const float DeltaTime) const
{
	/**
	* Approximates the movement by finding the average acceleration:
	* avg_a = (v_final - v_initial)/t
	* Uses that average acceleration in the kinematic displacement formula
	* p - p0 = v_initial*t + 0.5*avg_a*(t^2)
	* Substituting in the average acceleration expression results in:
	* p - p0 = 0.5*t*(v_initial + v_final)
	*/
	return 0.5*DeltaTime*(InitVelocity + FinalVelocity);
}