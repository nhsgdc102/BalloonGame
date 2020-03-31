// Copyright 2020 Northview High School Game Development Club

#pragma once

#include "CoreMinimal.h"
#include "Balloon.h"

#include "CableComponent.h"
#include "Kismet/GameplayStatics.h" //For main player pawn
#include "GameFramework/ProjectileMovementComponent.h"

#include "AdvancedBalloon.generated.h"

class UStaticMeshComponent;
class UCableComponent;
class UGameplayStatics;
class UProjectileMovementComponent;

/**
 * 
 */
UCLASS()
class BALLOONGAME_API AAdvancedBalloon : public ABalloon
{
	GENERATED_BODY()

public: 
	// Constructor--sets default values
	AAdvancedBalloon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when actor is removed from level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called after components have been initialized in C++ and then Blueprints
	virtual void PostInitializeComponents() override;

// Mesh Component
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		//Component serving as balloon's string
		UStaticMeshComponent* mesh;
	static FName MeshName;

public:
	UFUNCTION()
		void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnMeshEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

// Cable Component
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		//Component serving as balloon's string
		UCableComponent* string;
	static FName CableName;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable Substep")
		//Cable substep at distance of maxSpawnRadius
		float farCableSubstep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable Substep")
		//Cable substep at distance of 0
		float closeCableSubstep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable Substep")
		//Duplicate from game mode
		float maxSpawnRadius;

public:
	UFUNCTION(BlueprintCallable)
		//Used to set cable substep from blueprint construction script, implement into C++ constructor once parameters are finalized
		void setCableSubstep();

// Spawning spawn indicator
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Indicator")
		//spawn indicator blueprint class--to be set in class defaults
		TSubclassOf<AActor> SpawnIndicatorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Indicator")
		//Relative offset from spawning location of balloon
		FVector SpawnOffset;

private:
	AActor* spawnIndicator;

// Projectile Component
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		//Component that moves balloon upward
		UProjectileMovementComponent* projComp;
	static FName ProjCompName;
};
