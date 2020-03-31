// Copyright 2020 Northview High School Game Development Club


#include "AdvancedBalloon.h"

//For casting to trigger box in end overlap
#include "Engine/TriggerBox.h"

//For increasing score or number of escaped balloons
#include "Balloon_GI.h"

class ATriggerBox;
class UGameInstance;
class UBalloon_GI;

FName AAdvancedBalloon::MeshName(TEXT("Balloon Mesh"));
FName AAdvancedBalloon::CableName(TEXT("String"));
FName AAdvancedBalloon::ProjCompName(TEXT("Projectile Component"));

// Sets default values
AAdvancedBalloon::AAdvancedBalloon()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh Setup
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(AAdvancedBalloon::MeshName);
	RootComponent = mesh; //mesh is the root of the balloon; cable is attached to it.
	mesh->SetGenerateOverlapEvents(true);
	mesh->SetCollisionProfileName("Balloon");
	//mesh->OnComponentHit.AddDynamic(this, &AAdvancedBalloon::OnMeshHit);
	//mesh->OnComponentEndOverlap.AddDynamic(this, &AAdvancedBalloon::OnMeshEndOverlap);

	//Cable Setup
	string = CreateDefaultSubobject<UCableComponent>(AAdvancedBalloon::CableName);
	string->SetupAttachment(RootComponent);
	farCableSubstep = 0.06f;
	closeCableSubstep = 0.02f;
	maxSpawnRadius = 1000.f;
	//setCableSubstep();
	//More Details
	string->bAttachStart = true;
	string->SetWorldLocation(mesh->GetSocketLocation(TEXT("CableStart"))); //sets cable at socket location in balloon mesh
	string->bAttachEnd = false;
	string->NumSegments = 4;
	string->CableGravityScale = 2.5f;
	string->CableWidth = 2.f;
	string->NumSides = 3;
	string->SetGenerateOverlapEvents(false);
	string->SetCollisionProfileName("NoCollision");

	//Spawn Indicator
	SpawnOffset = FVector(0.f, 0.f, 109.f);

	//Projectile Comp Setup
	projComp = CreateDefaultSubobject<UProjectileMovementComponent>(AAdvancedBalloon::ProjCompName);
	projComp->MaxSpeed = 100.f;
	projComp->ProjectileGravityScale = -1.f;
	projComp->bShouldBounce = true;
	projComp->Velocity = FVector(0.f);
}

void AAdvancedBalloon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Sets component properties after they have been initialized using blueprint changes

	//Sets up mesh collision
	mesh->OnComponentHit.AddDynamic(this, &AAdvancedBalloon::OnMeshHit);
	mesh->OnComponentEndOverlap.AddDynamic(this, &AAdvancedBalloon::OnMeshEndOverlap);

	//Sets up cable substep
	setCableSubstep();
}

void AAdvancedBalloon::setCableSubstep()
{
	float distance = GetHorizontalDistanceTo(UGameplayStatics::GetPlayerPawn(this, 0));
	float substep = distance * (farCableSubstep - closeCableSubstep) / maxSpawnRadius + closeCableSubstep;
	string->SubstepTime = substep;
}

void AAdvancedBalloon::BeginPlay()
{
	Super::BeginPlay();

	//Spawns spawn indicator object in world
	if (IsValid(SpawnIndicatorClass))
	{
		FTransform spawnTransform = FTransform(FRotator(), GetActorLocation() + SpawnOffset, FVector(1.f));
		spawnIndicator = GetWorld()->SpawnActor<AActor>(SpawnIndicatorClass, spawnTransform);
	}
}

// Called when actor is removed from level
void AAdvancedBalloon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	spawnIndicator->Destroy();
}

// Called when rock hits balloon mesh
void AAdvancedBalloon::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{
		if(bDebugCollision)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("BalloonHit"));
		OtherActor->Destroy();

		//Adds to score in game instance--may want to replace with interfaced version
		/*
		UBalloon_GI* gameInstance = Cast<UBalloon_GI, UGameInstance>(UGameplayStatics::GetGameInstance(this));
		if (IsValid(gameInstance))
			gameInstance->addToScore(scoreAddition);
		*/

		string->bAttachStart = false; //allows string to fall by itself after the balloon pops
		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //prevents more collisions
		callDeath(true); //handles calling a timer for death
	}
}

// Called when balloon leaves bounding volume
void AAdvancedBalloon::OnMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ATriggerBox* trigger = (OtherActor) ? Cast<ATriggerBox, AActor>(OtherActor) : NULL; //only casts if OtherActor ptr is not null 

	if (IsValid(trigger))
	{
		if (bDebugCollision)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, TEXT("BalloonEndOverlap"));
		//Increments number of escaped in game instance--may want to replace with interfaced version
		/*
		UBalloon_GI* gameInstance = Cast<UBalloon_GI, UGameInstance>(UGameplayStatics::GetGameInstance(this));
		if (IsValid(gameInstance))
			gameInstance->incrementEscaped();
		*/

		mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //prevents more collisions
		callDeath(false); //handles calling a timer for death, does not make a popping sound
	}
}