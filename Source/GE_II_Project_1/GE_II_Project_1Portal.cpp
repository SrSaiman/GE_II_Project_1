// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_II_Project_1Portal.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "GE_II_Project_1Character.h"
#include "Math/Vector.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

#define COLLISION_VIEWABLE ECC_GameTraceChannel4

// Sets default values
AGE_II_Project_1Portal::AGE_II_Project_1Portal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Portal"));

	// Portal
	//--------------------------------------------------------------------------------

	Portal_Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Portal_Scene"));
	Portal_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal_Mesh"));
	Camera_Portal = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Camera_Portal"));
	Portal_Mesh_Border = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal_Mesh_Border"));

	Dummy_Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy_Portal"));
	Left_Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Left_Portal"));
	Right_Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Right_Portal"));
	Up_Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Up_Portal"));
	Down_Portal = CreateDefaultSubobject<USceneComponent>(TEXT("Down_Portal"));

	Portal_Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0f));
	Portal_Mesh->SetRelativeScale3D(FVector(2, 2, 2));
	Portal_Mesh_Border->SetRelativeScale3D(FVector(2, 2, 2));
	Portal_Scene->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));

	Left_Portal->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
	Right_Portal->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
	Up_Portal->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));
	Down_Portal->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));

	RootComponent = Portal;

	Portal_Scene->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Portal->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh_Border->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Dummy_Portal->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Left_Portal->AttachToComponent(Dummy_Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Right_Portal->AttachToComponent(Dummy_Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Up_Portal->AttachToComponent(Dummy_Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Down_Portal->AttachToComponent(Dummy_Portal, FAttachmentTransformRules::KeepRelativeTransform);

	Left_Portal->SetRelativeLocation(FVector(25.0f, 90.0f, 0.0f));
	Right_Portal->SetRelativeLocation(FVector(25.0f, -90.0f, 0.0f));
	Up_Portal->SetRelativeLocation(FVector(25.0f, 0.0f, 120.0f));
	Down_Portal->SetRelativeLocation(FVector(25.0f, -0.0f, -120.0f));

	Camera_Portal->bOverride_CustomNearClippingPlane = true;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
}

// Called when the game starts or when spawned
void AGE_II_Project_1Portal::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	PlayerCamera = PlayerController->PlayerCameraManager;

	Portal_Mesh->OnComponentBeginOverlap.AddDynamic(this, &AGE_II_Project_1Portal::OnBeginOverlapPortal);
	Portal_Mesh->OnComponentEndOverlap.AddDynamic(this, &AGE_II_Project_1Portal::OnEndOverlapPortal);

	Camera_Portal->TextureTarget->ResizeTarget(GEngine->GameViewport->Viewport->GetSizeXY().X, GEngine->GameViewport->Viewport->GetSizeXY().Y);

	Portal_Mesh->SetVisibility(false);
	Portal_Mesh_Border->SetVisibility(false);
}

// Called every frame
void AGE_II_Project_1Portal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Linked_Portal != nullptr)
	{
		FTransform Camera = UKismetMathLibrary::MakeRelativeTransform(FTransform(PlayerCamera->GetTransform()), FTransform(Portal_Scene->GetComponentTransform()));

		Linked_Portal->Camera_Portal->SetRelativeLocationAndRotation(FVector(Camera.GetLocation()),FRotator(Camera.GetRotation()));

		Linked_Portal->Camera_Portal->CustomNearClippingPlane = 1.0f+ UKismetMathLibrary::Vector_Distance(FVector(PlayerCamera->GetCameraLocation()),FVector(GetActorLocation()));
	}

	// ercorrer a lista de players usando um loop foreach
	for (AGE_II_Project_1Character* PlayerCharacter : players)
	{
		Check_if_Player_Should_Teleport(PlayerCharacter);
	}
}

void AGE_II_Project_1Portal::Check_if_Player_Should_Teleport(AGE_II_Project_1Character* playerInArray)
{
	// Verifique se o jogador está no array antes de tentar removê-lo
	if (players.Contains(playerInArray))
	{
		FVector DirecaoJogador = FVector((playerInArray->GetActorLocation() + FVector((playerInArray->GetVelocity() * GetWorld()->GetDeltaSeconds()))) - GetActorLocation());
		FVector DirecaoFrenteAtual = GetActorForwardVector();
		float firstCheck = UKismetMathLibrary::Dot_VectorVector(DirecaoJogador, DirecaoFrenteAtual.GetSafeNormal());


		float secondCheck = UKismetMathLibrary::Dot_VectorVector(playerInArray->GetLastMovementInputVector().GetSafeNormal(), GetActorForwardVector());

		if (firstCheck <= 0 && secondCheck < 0)
		{
			Teleport_Player(playerInArray);
		}
	}
}


void AGE_II_Project_1Portal::Teleport_Player(AGE_II_Project_1Character* playerToTeleport)
{

	// Verifique se o jogador está no array antes de tentar removê-lo
	if (players.Contains(playerToTeleport))
	{
		FVector RelativeVelocity = UKismetMathLibrary::InverseTransformDirection(playerToTeleport->GetTransform(), playerToTeleport->GetVelocity());

		FTransform ComposeTransforms = UKismetMathLibrary::ComposeTransforms(UKismetMathLibrary::MakeRelativeTransform(playerToTeleport->GetFirstPersonCameraComponent()->GetComponentTransform(), Portal_Scene->GetComponentTransform()), Linked_Portal->GetActorTransform());
		FVector Minus = (ComposeTransforms.GetLocation() - playerToTeleport->GetFirstPersonCameraComponent()->GetRelativeLocation());
		FVector PortalForwardVector = (Linked_Portal->GetActorForwardVector() * 10);
		FVector newLocation = FVector(PortalForwardVector + Minus);

		playerToTeleport->SetActorLocation(newLocation);

		FRotator Rotator(0.0f, ComposeTransforms.Rotator().Yaw, ComposeTransforms.Rotator().Roll);
		playerToTeleport->GetController()->SetControlRotation(Rotator);


		playerToTeleport->GetCharacterMovement()->Velocity = (UKismetMathLibrary::TransformDirection(UKismetMathLibrary::MakeTransform(playerToTeleport->GetActorLocation(), playerToTeleport->GetControlRotation()), RelativeVelocity));
	}
}

void AGE_II_Project_1Portal::OnEndOverlapPortal(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Linked_Portal)
	{
			if (AGE_II_Project_1Character* PlayerCharacter = Cast<AGE_II_Project_1Character>(OtherActor))
			{
				if (PlayerCharacter)
				{
					UCapsuleComponent* CapsuleComponent = PlayerCharacter->GetCapsuleComponent();

					// Verifique se o array players não está vazio antes de tentar acessá-lo
					if (players.Num() > 0)
					{
						// Verifique se o jogador está no array antes de tentar removê-lo
						if (players.Contains(PlayerCharacter))
						{
							players.Remove(PlayerCharacter);
						}
					}
				}
			}
	}
}

void AGE_II_Project_1Portal::OnBeginOverlapPortal(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Linked_Portal)
	{
		if (OtherActor->IsA<AGE_II_Project_1Character>())
		{
			if (AGE_II_Project_1Character* PlayerCharacter = Cast<AGE_II_Project_1Character>(OtherActor))
			{
				players.AddUnique(PlayerCharacter);
				if (players.IndexOfByKey(PlayerCharacter) != -1)
				{
					UCapsuleComponent* CapsuleComponent = PlayerCharacter->GetCapsuleComponent();

				}
			}
		}
	}
}

void AGE_II_Project_1Portal::Place_Dummy_Portal(FRotator DummyRotationPortal, FVector DummyLocationPortal)
{
	Dummy_Portal->SetWorldLocationAndRotation(DummyLocationPortal, DummyRotationPortal);
	Move_Dummy_Portal();
}

void AGE_II_Project_1Portal::Change_Location(FRotator RotationPortal, FVector LocationPortal)
{
	Portal_Mesh->SetVisibility(true);
	Portal_Mesh_Border->SetVisibility(true);
	SetActorLocationAndRotation(LocationPortal, RotationPortal);
}

void AGE_II_Project_1Portal::Link(AGE_II_Project_1Portal* PortalToLinked_Portal)
{
	Linked_Portal = PortalToLinked_Portal;
}

void AGE_II_Project_1Portal::Move_Dummy_Portal()
{
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Left_Portal->GetComponentLocation(), (Left_Portal->GetForwardVector() * 50) + Left_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Left_Portal->GetComponentLocation(), (Left_Portal->GetForwardVector() * 50) + Left_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
	{
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Right_Portal->GetComponentLocation(), (Right_Portal->GetForwardVector() * 50) + Right_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Right_Portal->GetComponentLocation(), (Right_Portal->GetForwardVector() * 50) + Right_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
		{
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Up_Portal->GetComponentLocation(), (Up_Portal->GetForwardVector() * 50) + Up_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Up_Portal->GetComponentLocation(), (Up_Portal->GetForwardVector() * 50) + Up_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
			{
				if (GetWorld()->LineTraceSingleByChannel(HitResult, Down_Portal->GetComponentLocation(), (Down_Portal->GetForwardVector() * 50) + Down_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Down_Portal->GetComponentLocation(), (Down_Portal->GetForwardVector() * 50) + Down_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
				{
					Change_Location(FRotator(Dummy_Portal->GetComponentRotation()), FVector(Dummy_Portal->GetComponentLocation()));
				}
				else
				{
					Dummy_Portal->SetWorldLocation(Dummy_Portal->GetComponentLocation() + Dummy_Portal->GetUpVector() * 10);
					DrawDebugLine(GetWorld(), Down_Portal->GetComponentLocation(), (Down_Portal->GetForwardVector() * 50) + Down_Portal->GetComponentLocation(), FColor::Red, true, 5.f, 0, 1.0f);
					if (GetWorld()->LineTraceSingleByChannel(HitResult, Up_Portal->GetComponentLocation(), (Up_Portal->GetForwardVector() * 50) + Up_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Up_Portal->GetComponentLocation(), (Up_Portal->GetForwardVector() * 50) + Up_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
					{
						Move_Dummy_Portal();
					}
				}
			}
			else
			{
				Dummy_Portal->SetWorldLocation(Dummy_Portal->GetComponentLocation() + Dummy_Portal->GetUpVector() * -10);
				DrawDebugLine(GetWorld(), Up_Portal->GetComponentLocation(), (Up_Portal->GetForwardVector() * 50) + Up_Portal->GetComponentLocation(), FColor::Red, true, 5.f, 0, 1.0f);
				Move_Dummy_Portal();
			}
		}
		else
		{
			Dummy_Portal->SetWorldLocation(Dummy_Portal->GetComponentLocation() + Dummy_Portal->GetRightVector() * 10);
			DrawDebugLine(GetWorld(), Right_Portal->GetComponentLocation(), (Right_Portal->GetForwardVector() * 50) + Right_Portal->GetComponentLocation(), FColor::Red, true, 5.f, 0, 1.0f);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Left_Portal->GetComponentLocation(), (Left_Portal->GetForwardVector() * 50) + Left_Portal->GetComponentLocation(), ECollisionChannel::ECC_GameTraceChannel2) && !GetWorld()->LineTraceSingleByObjectType(HitResult, Left_Portal->GetComponentLocation(), (Left_Portal->GetForwardVector() * 50) + Left_Portal->GetComponentLocation(), FCollisionObjectQueryParams(ECC_WorldStatic), TraceParams))
			{
				Move_Dummy_Portal();
			}
		}
	}
	else
	{
		Dummy_Portal->SetWorldLocation(Dummy_Portal->GetComponentLocation() + Dummy_Portal->GetRightVector() * -10);
		DrawDebugLine(GetWorld(), Left_Portal->GetComponentLocation(), (Left_Portal->GetForwardVector() * 50) + Left_Portal->GetComponentLocation(), FColor::Red, true, 5.f, 0, 1.0f);
		Move_Dummy_Portal();
	}
}