// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_II_Project_1Portals.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"
#include "BPI_Jump_Portals.h"
#include "GE_II_Project_1Character.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"

// Sets default values
AGE_II_Project_1Portals::AGE_II_Project_1Portals()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//--------------------------------------------------------------------------------
	Portals = CreateDefaultSubobject<USceneComponent>(TEXT("Portals"));

	// Portal 1
	//--------------------------------------------------------------------------------
	Portal_Blue = CreateDefaultSubobject<USceneComponent>(TEXT("Portal_Blue"));
	Camera_Root_Blue = CreateDefaultSubobject<USceneComponent>(TEXT("Camera_Root_Blue"));
	Portal_Scene_Blue = CreateDefaultSubobject<USceneComponent>(TEXT("Portal_Scene_Blue"));
	Trigger_Blue = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger_Blue"));
	Portal_Mesh_Blue = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal_Mesh_Blue"));
	Camera_Portal_Orange = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera_Portal_Orange"));
	LookAt_Blue = CreateDefaultSubobject<USceneComponent>(TEXT("LookAt_Blue"));
	//Portal 2
	//-------------------------------------------------------------------------------
	Portal_Orange = CreateDefaultSubobject<USceneComponent>(TEXT("Portal_Orange"));
	Camera_Root_Orange = CreateDefaultSubobject<USceneComponent>(TEXT("Camera_Root_Orange"));
	Portal_Scene_Orange = CreateDefaultSubobject<USceneComponent>(TEXT("Portal_Scene_Orange"));
	Trigger_Orange = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger_Orange"));
	Portal_Mesh_Orange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal_Mesh_Orange"));
	Camera_Portal_Blue = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera_Portal_Blue"));
	LookAt_Orange = CreateDefaultSubobject<USceneComponent>(TEXT("LookAt_Orange"));
	//-------------------------------------------------------------------------------
	Portal_Mesh_Blue->SetRelativeRotation(FRotator(0.f, 90.f, 90.f));
	Portal_Mesh_Orange->SetRelativeRotation(FRotator(0.f, 90.f, 90.f));
	Camera_Root_Blue->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));
	Camera_Root_Orange->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));
	Trigger_Blue->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	Trigger_Orange->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	RootComponent = Portals;

	//Portal 1
	//-------------------------------------------------------------------------------
	Portal_Blue->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger_Blue->AttachToComponent(Portal_Blue, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Root_Blue->AttachToComponent(Portal_Blue, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Scene_Blue->AttachToComponent(Portal_Blue, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh_Blue->AttachToComponent(Portal_Blue, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Portal_Orange->AttachToComponent(Camera_Root_Blue, FAttachmentTransformRules::KeepRelativeTransform);
	LookAt_Blue->AttachToComponent(Camera_Root_Blue, FAttachmentTransformRules::KeepRelativeTransform);

	//Portal 2
	//-------------------------------------------------------------------------------
	Portal_Orange->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger_Orange->AttachToComponent(Portal_Orange, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Root_Orange->AttachToComponent(Portal_Orange, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Scene_Orange->AttachToComponent(Portal_Orange, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh_Orange->AttachToComponent(Portal_Orange, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Portal_Blue->AttachToComponent(Camera_Root_Orange, FAttachmentTransformRules::KeepRelativeTransform);
	LookAt_Orange->AttachToComponent(Camera_Root_Orange, FAttachmentTransformRules::KeepRelativeTransform);

	Trigger_Blue->OnComponentBeginOverlap.AddDynamic(this, &AGE_II_Project_1Portals::OnBeginOverlapBlue);
	Trigger_Blue->OnComponentEndOverlap.AddDynamic(this, &AGE_II_Project_1Portals::OnEndOverlapBlue);

	Trigger_Orange->OnComponentBeginOverlap.AddDynamic(this, &AGE_II_Project_1Portals::OnBeginOverlapOrange);
	Trigger_Orange->OnComponentEndOverlap.AddDynamic(this, &AGE_II_Project_1Portals::OnEndOverlapOrange);
}

// Called when the game starts or when spawned
void AGE_II_Project_1Portals::BeginPlay()
{
	Super::BeginPlay();

	// Check if World is valid before accessing it
	if (UWorld* World = GetWorld())
	{
		// Check if PlayerController is valid before accessing it
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// Check if PlayerCameraManager is valid before accessing it
			PlayerCamera = PlayerController->PlayerCameraManager;
			if (!PlayerCamera)
			{
				// Log a warning if PlayerCameraManager is null
				UE_LOG(LogTemp, Warning, TEXT("PlayerCameraManager is nullptr"));
			}
		}
		else
		{
			// Log a warning if PlayerController is null
			UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
		}
	}
	else
	{
		// Log a warning if World is null
		UE_LOG(LogTemp, Warning, TEXT("World is nullptr"));
	}
}

// Called every frame
void AGE_II_Project_1Portals::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Portal_Scene_Blue->SetWorldLocation(FVector(PlayerCamera->GetCameraLocation()));
	Portal_Scene_Orange->SetWorldLocation(FVector(PlayerCamera->GetCameraLocation()));

	Camera_Portal_Blue->SetRelativeLocation(FVector(Portal_Scene_Blue->GetRelativeLocation()));
	Camera_Portal_Blue->SetWorldRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(Camera_Portal_Blue->GetComponentLocation(), LookAt_Orange->GetComponentLocation())));
	Camera_Portal_Blue->FOVAngle = UKismetMathLibrary::Clamp(UKismetMathLibrary::DegAtan(200.f/ UKismetMathLibrary::Max(UKismetMathLibrary::VSize(FVector(Camera_Portal_Blue->GetRelativeLocation())), 1.f)), 5.0f, 100.0f);

	Camera_Portal_Orange->SetRelativeLocation(FVector(Portal_Scene_Orange->GetRelativeLocation()));
	Camera_Portal_Orange->SetWorldRotation(FRotator(UKismetMathLibrary::FindLookAtRotation(Camera_Portal_Orange->GetComponentLocation(), LookAt_Blue->GetComponentLocation())));
	Camera_Portal_Orange->FOVAngle = UKismetMathLibrary::Clamp(UKismetMathLibrary::DegAtan(200.f / UKismetMathLibrary::Max(UKismetMathLibrary::VSize(FVector(Camera_Portal_Orange->GetRelativeLocation())), 1.f)), 5.0f, 100.0f);
}

void AGE_II_Project_1Portals::OnBeginOverlapBlue(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp != Trigger_NotCollided)
	{
		Trigger_NotCollided = Trigger_Orange;
		if (OtherActor->GetClass()->ImplementsInterface(UBPI_Jump_Portals::StaticClass()))
		{
			IBPI_Jump_Portals::Execute_JumpPortal(OtherActor,FVector(Trigger_NotCollided->GetComponentLocation()),FRotator(Camera_Portal_Blue->GetComponentRotation()));
		}

	}
}

void AGE_II_Project_1Portals::OnBeginOverlapOrange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComp != Trigger_NotCollided)
	{
		Trigger_NotCollided = Trigger_Blue;
		if (OtherActor->GetClass()->ImplementsInterface(UBPI_Jump_Portals::StaticClass()))
		{
			IBPI_Jump_Portals::Execute_JumpPortal(OtherActor, FVector(Trigger_NotCollided->GetComponentLocation()), FRotator(Camera_Portal_Orange->GetComponentRotation()));
		}

	}
}

void AGE_II_Project_1Portals::OnEndOverlapBlue(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedComp == Trigger_NotCollided)
	{
		Trigger_NotCollided = nullptr;
	}
}

void AGE_II_Project_1Portals::OnEndOverlapOrange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlappedComp == Trigger_NotCollided)
	{
		Trigger_NotCollided = nullptr;
	}
}


void AGE_II_Project_1Portals::IsBlue(FRotator SpawnRotation, FVector  SpawnLocation)
{
	IsOrangePortal = false;
	Portal_Blue->SetWorldLocationAndRotation(SpawnLocation, SpawnRotation);
}
void AGE_II_Project_1Portals::IsOrange(FRotator SpawnRotation, FVector  SpawnLocation)
{
	IsOrangePortal = true;
	Portal_Orange->SetWorldLocationAndRotation(SpawnLocation, SpawnRotation);
}