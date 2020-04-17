// Copyright 2020 Northview High School Game Development Club


#include "CP_Slingshot.h"

/*Additional include statements*/
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACP_Slingshot::ACP_Slingshot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*Component initializations*/
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComp);
	MeshComp->SetCollisionProfileName("Slingshot");
	MeshComp->SetGenerateOverlapEvents(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(GetRootComponent());
	CameraComp->SetRelativeLocation(FVector(-80.3f, 0.f, 24.5f));

}

// Called when the game starts or when spawned
void ACP_Slingshot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACP_Slingshot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACP_Slingshot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*Camera*/
	PlayerInputComponent->BindAxis("Yaw", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Pitch", this, &APawn::AddControllerPitchInput);

	/*Interacting with Slingshot*/
	PlayerInputComponent->BindAxis("Pull", this, &ACP_Slingshot::PullSlingshot);
}

// Binded to player input to change how far back slingshot is pulled
void ACP_Slingshot::PullSlingshot(float Val)
{
	//Fill body
}