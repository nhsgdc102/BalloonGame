// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h" //For collision sphere
#include "Components/StaticMeshComponent.h" //For rock mesh
#include "GameFramework/ProjectileMovementComponent.h" //For projectile movement

#include "MovableRock.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class BALLOONGAME_API AMovableRock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableRock();

protected:
	// Called after components have been initialized in C++ and then Blueprints
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//Components
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		//Serves as root component
		USphereComponent* collisionSphere;
	static FName colSphereName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* rockMesh;
	static FName rockMeshName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* rockMovement;
	static FName rockMovementName;

//Collision
public:
	UFUNCTION()
		// Called when rock hits balloon or some other object
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintCallable)
		// Stops bounce and spawns another duplicate rock when rock hits balloon
		bool onBounce();
	UFUNCTION()
		// Called when rock exits trigger volume
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

/*
//Lifespan -- preventing rock from going too far or existing too long
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lifespan")
		//Maximum distance from initial location
		float maxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lifespan")
		//Maximum time after spawning
		float maxTime;

private:
	FVector initLocation;
*/

//Projectile Behavior
public:
	//To be used by owning Slingshot obj
	void setRockVelocity(FVector velocity);

	UFUNCTION()
		//Called when rock becomes stationary on floor
		void OnStop(const FHitResult& ImpactResult);

//Debugging
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging")
		bool bDebugCollision;
};
