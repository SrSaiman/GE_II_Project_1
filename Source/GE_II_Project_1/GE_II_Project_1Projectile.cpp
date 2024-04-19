// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_II_Project_1Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AGE_II_Project_1Projectile::AGE_II_Project_1Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(20.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGE_II_Project_1Projectile::OnHit);		// set up a notification for when this component hits something blocking


	static ConstructorHelpers::FClassFinder<AActor>Mirror1(TEXT("/Game/FirstPerson/Blueprints/NewBlueprint"));
	if (Mirror1.Class != NULL)
	{
		Mirror = Mirror1.Class;
	}


	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	//projectile mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetRelativeScale3D(FVector(0.125f, 0.125f, 0.125f));
	ProjectileMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;

}

void AGE_II_Project_1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UWorld* const World = GetWorld();
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor->ActorHasTag(TEXT("Can_Collide"))))
	{
		FVector ImpactNormal = Hit.ImpactNormal;

		const FRotator SpawnRotation = ImpactNormal.Rotation();

		const FVector SpawnLocation = Hit.ImpactPoint;

		World->SpawnActor<AActor>(Mirror, SpawnLocation, SpawnRotation);
		Destroy();
	}
}