// Fill out your copyright notice in the Description page of Project Settings.


#include "GE_II_Project_1Portal.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "GE_II_Project_1Character.h"
#include "Math/Vector.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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

	Portal_Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 90.0f));
	Portal_Mesh->SetRelativeScale3D(FVector(2, 2, 2));
	Portal_Mesh_Border->SetRelativeScale3D(FVector(2, 2, 2));
	Portal_Scene->SetRelativeRotation(FRotator(0.0f, -180.0f, 0.0f));

	RootComponent = Portal;

	Portal_Scene->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Camera_Portal->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);
	Portal_Mesh_Border->AttachToComponent(Portal, FAttachmentTransformRules::KeepRelativeTransform);

	Camera_Portal->bOverride_CustomNearClippingPlane = true;

	Portal_Mesh->OnComponentBeginOverlap.AddDynamic(this, &AGE_II_Project_1Portal::OnBeginOverlapPortal);
	Portal_Mesh->OnComponentEndOverlap.AddDynamic(this, &AGE_II_Project_1Portal::OnEndOverlapPortal);
}

// Called when the game starts or when spawned
void AGE_II_Project_1Portal::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World->GetFirstPlayerController();
	PlayerCamera = PlayerController->PlayerCameraManager;

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

	for (int i = 0; i != players.Num(); ++i) {
		Check_if_Player_Should_Teleport(players[i]);
	}
}

void AGE_II_Project_1Portal::Check_if_Player_Should_Teleport(AGE_II_Project_1Character* playerInArray)
{
	FVector DirecaoJogador = FVector((playerInArray->GetActorLocation() + FVector((playerInArray->GetVelocity() * GetWorld()->GetDeltaSeconds()))) - GetActorLocation());
	FVector DirecaoFrenteAtual = GetActorForwardVector();
	float firstCheck = UKismetMathLibrary::Dot_VectorVector(DirecaoJogador, DirecaoFrenteAtual.GetSafeNormal());


	float secondCheck = UKismetMathLibrary::Dot_VectorVector(playerInArray->GetLastMovementInputVector().GetSafeNormal(),GetActorForwardVector());

	if (firstCheck <= 0 && secondCheck<0)
	{
		Teleport_Player(playerInArray);
	}
}

void AGE_II_Project_1Portal::Teleport_Player(AGE_II_Project_1Character* playerToTeleport)
{
	FVector RelativeVelocity = UKismetMathLibrary::InverseTransformDirection(playerToTeleport->GetTransform(), playerToTeleport->GetVelocity());

	FTransform ComposeTransforms = UKismetMathLibrary::ComposeTransforms(UKismetMathLibrary::MakeRelativeTransform(playerToTeleport->GetFirstPersonCameraComponent()->GetComponentTransform(), Portal_Scene->GetComponentTransform()), Linked_Portal->GetActorTransform());
	FVector Minus= (ComposeTransforms.GetLocation() - playerToTeleport->GetFirstPersonCameraComponent()->GetRelativeLocation());
	FVector PortalForwardVector = (Linked_Portal->GetActorForwardVector() * 10);
	FVector newLocation = FVector(PortalForwardVector + Minus);
	playerToTeleport->SetActorLocation(newLocation);

	FRotator Rotator(0.0f, ComposeTransforms.Rotator().Yaw, ComposeTransforms.Rotator().Roll);
	playerToTeleport->GetController()->SetControlRotation(Rotator);


	playerToTeleport->GetCharacterMovement()->Velocity = (UKismetMathLibrary::TransformDirection(UKismetMathLibrary::MakeTransform(playerToTeleport->GetActorLocation(), playerToTeleport->GetControlRotation()), RelativeVelocity));
}

void AGE_II_Project_1Portal::OnEndOverlapPortal(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	oi = true;

	AGE_II_Project_1Character* PlayerCharacter = Cast<AGE_II_Project_1Character>(OtherActor);
	if (players.Contains(PlayerCharacter))
	{
		players.Remove(PlayerCharacter);
	}
	UCapsuleComponent* CapsuleComponent = PlayerCharacter->GetCapsuleComponent();

	if (CapsuleComponent)
	{
		CapsuleComponent->SetCollisionProfileName("Pawn");
	}
}

void AGE_II_Project_1Portal::OnBeginOverlapPortal(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	oi = true;
	if (Linked_Portal)
	{
		AGE_II_Project_1Character* PlayerCharacter = Cast<AGE_II_Project_1Character>(OtherActor);
		players.AddUnique(PlayerCharacter);
		if (players.IndexOfByKey(PlayerCharacter) != -1)
		{
			UCapsuleComponent* CapsuleComponent = PlayerCharacter->GetCapsuleComponent();

			if (CapsuleComponent)
			{
				CapsuleComponent->SetCollisionProfileName("PortalPawn");
			}
		}
	}
}