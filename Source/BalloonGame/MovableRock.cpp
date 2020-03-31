// Copyright 2020 Northview High School Game Development Club


#include "MovableRock.h"

#include "Balloon.h" //For seeing if OtherActor is balloon in OnHit

class ABalloon;

FName AMovableRock::colSphereName(TEXT("collisionSphere"));
FName AMovableRock::rockMeshName(TEXT("rockMesh"));
FName AMovableRock::rockMovementName(TEXT("rockMovement"));

// Sets default values
AMovableRock::AMovableRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collisionSphere = CreateDefaultSubobject<USphereComponent>(AMovableRock::colSphereName);
	RootComponent = collisionSphere; //for projectile bounce to work correctly, it is better for collision component (sphere) to be the root
	rockMesh = CreateDefaultSubobject<UStaticMeshComponent>(AMovableRock::rockMeshName);
	rockMesh->SetupAttachment(RootComponent); //rock mesh will move with collisionSphere
	rockMovement = CreateDefaultSubobject<UProjectileMovementComponent>(AMovableRock::rockMovementName); 
	//rockMovement does not have location so no need for attachment

	//Setting up projectile values
	rockMovement->bShouldBounce = true;
	rockMovement->bBounceAngleAffectsFriction = true;
	rockMovement->Bounciness = 0.5f;
	rockMovement->MinFrictionFraction = 1.0f;
	rockMovement->Velocity = FVector(0.f);
	rockMovement->bAutoActivate = false; //Only becomes active after velocity and initial speed are set from Slingshot obj

	//Setting up collision
	collisionSphere->Mobility = EComponentMobility::Movable;
	collisionSphere->SetSphereRadius(6.526853f);
	collisionSphere->SetGenerateOverlapEvents(true);
	collisionSphere->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; //No need for stepping on rock
	collisionSphere->SetCollisionProfileName("Rock");

	//Setting up rock
	rockMesh->Mobility = EComponentMobility::Movable;
	rockMesh->SetGenerateOverlapEvents(false);
	rockMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No; //No need for stepping on rock
	rockMesh->SetCollisionProfileName("NoCollision");

	/*
	//Other defaults
	maxDistance = 3000.f;
	maxTime = 3.f;
	initLocation = FVector(0.f);
	*/

	bDebugCollision = true;
}

// Called after components have been initialized in C++ and then Blueprints
void AMovableRock::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sets component properties after they have been initialized using blueprint changes

	//Sets up rock's sphere collision binding
	collisionSphere->OnComponentHit.AddDynamic(this, &AMovableRock::OnHit);
	collisionSphere->OnComponentEndOverlap.AddDynamic(this, &AMovableRock::OnEndOverlap);

	//Sets up projectile comp binding
	rockMovement->OnProjectileStop.AddDynamic(this, &AMovableRock::OnStop);
}

// Called when the game starts or when spawned
void AMovableRock::BeginPlay()
{
	Super::BeginPlay();

	/*
	//Sets initial location -- used to find distance from start
	initLocation = this->GetActorLocation();
	
	//Sets lifespan according to maxTime
	this->SetLifeSpan(maxTime);
	*/
}

// Called every frame
void AMovableRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	//Destroys rock if it is too far away
	float distance = (this->GetActorLocation() - initLocation).Size();
	if (distance > maxDistance)
		this->Destroy();
	*/
}

void AMovableRock::setRockVelocity(FVector velocity)
{
	rockMovement->Velocity = velocity;
	rockMovement->InitialSpeed = velocity.Size();
	rockMovement->Activate(true); //Resets so that new initialSpeed is used
}

// Called when rock hits balloon or any other object
void AMovableRock::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Alternative way to test if an object is of a certain class (perhaps more efficient than casting)
	if (IsValid(OtherActor) && OtherActor->IsA<ABalloon>()) //Using IsValid first will short-circuit safely in case of null pointer
	{
		if (bDebugCollision)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("Rock Hit Balloon"));

		onBounce();
	}
}

bool AMovableRock::onBounce()
{
	//Creates new rock with the same forward velocity as the original to prevent bounce back
	//Original rock will be killed by balloon

	/*
	For some reason, trying to reverse the velocity after the bounce did not work
	*/

	FVector spawnLoc = this->GetActorLocation();
	FRotator spawnRot = this->GetActorRotation();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//Should not be able to collide anyway because balloon is destroyed

	AMovableRock* duplicateRock = GetWorld()->SpawnActor<AMovableRock>(spawnLoc, spawnRot, spawnParams);
	//No need for references beside actual parameters because values are not constant

	if (IsValid(duplicateRock)) //Only true if duplicateRock spawned successfully and still exists
	{
		duplicateRock->setRockVelocity(rockMovement->Velocity);
		return true;
	}

	return false;
}


// Called when rock exits trigger volume
void AMovableRock::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bDebugCollision)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("Rock End Overlap"));

	this->Destroy();
}

// Called when rock becomes stationary on floor
void AMovableRock::OnStop(const FHitResult& ImpactResult)
{
	SetLifeSpan(5.f);
}